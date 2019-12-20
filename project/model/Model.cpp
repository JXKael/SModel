#include "Model.h"

using namespace smodel;

Model::Model() : camera_ray(0, 0, 1),  has_parent(false) {
}

Model::Model(const std::string &name) : camera_ray(0, 0, 1), has_parent(false) {

}

Model::Model(const smodel::Model &model) {
    this->camera_ray = model.camera_ray;

    // Bones
    this->centers = model.centers;
    this->bones = model.bones;
    this->blocks = model.blocks;
    this->block_colors = model.block_colors;
    this->tangent_points = model.tangent_points;

    // DOFs
    this->dofs = model.dofs;
    this->theta = model.theta;

    this->has_parent = model.has_parent;
}

Model::~Model() {}

void Model::Clear() {
    this->centers.clear();
    this->bones.clear();
    this->blocks.clear();
    this->block_colors.clear();
    this->tangent_points.clear();

    this->dofs.clear();
    this->theta.clear();
}

// >>>--------------------------- 更新theta和Bones中的local & global
void Model::MoveToInit() {
    Move(GetInitThetas());
}

void Model::Move(const Thetas &thetas) {
    for (int i = 0; i < thetas.size(); i++) {
        this->theta[i] = thetas[i];
    }
    for (int i = 0; i < BonesSize(); i++) {
        bones[i].local = bones[i].init_local;
    }

    // cout << "move" << endl;
    Thetas rotateX(DofsSize(), 0); // flexion
    Thetas rotateZ(DofsSize(), 0); // abduction
    Thetas rotateY(DofsSize(), 0); // twist
    Thetas translation(DofsSize(), 0); // pose

    for (int i = 0; i < DofsSize(); ++i) {
        float theta_val = has_parent && dofs[i].free_type == kUnderCtrl ? dofs[i].init_val : thetas[i];
        if (dofs[i].type == DofType::kRotation) {
            if (dofs[i].axis_type == AxisType::kX)
                rotateX[i] = theta_val;
            else if (dofs[i].axis_type == AxisType::kY)
                rotateY[i] = theta_val;
            else if (dofs[i].axis_type == AxisType::kZ)
                rotateZ[i] = theta_val;
            else
                std::cout << "wrong axis" << std::endl;
        } else if (dofs[i].type == DofType::kTranslation) {
            translation[i] = theta_val;
        } else {
            std::cout << "wrong theta" << std::endl;
        }
    }

    //transform joints separately in order
    transformJoints(translation); // pose
    transformJoints(rotateX); // flexion
    transformJoints(rotateZ); // abduction
    transformJoints(rotateY); // twist
}

void Model::Move(const Bone &parent_bone) {
    for (Bone &bone : bones) {
        if (bone.isModelRoot()) {
            bone.global = parent_bone.global * bone.local;
            break;
        }
    }
    Move(this->theta);
}

void Model::transformJoints(const Thetas &thetas) {
    for (int i = 0; i < dofs.size(); ++i) {
        if (dofs[i].bone != nullptr) {
            switch (dofs[i].type) {
            case DofType::kTranslation:
                translate(*dofs[i].bone, dofs[i].axis * thetas[i]);
                break;
            case DofType::kRotation:
                rotate(*dofs[i].bone, dofs[i].axis, thetas[i]);
                break;
            default:
                break;
            }
        }
    }
}

void Model::translate(Bone &bone, const smodel::vec3 &t) {
    bone.local(0, 3) += t[0];
    bone.local(1, 3) += t[1];
    bone.local(2, 3) += t[2];
    updateBonesTransformMatrix(bone);
}

void Model::rotate(Bone &bone, const smodel::vec3 &axis, float angle) {
    Eigen::Matrix4f rotateM = Eigen::Transform<float, 3, Eigen::Isometry>(Eigen::AngleAxisf(angle, axis)).matrix();
    bone.local = bone.local * rotateM;
    updateBonesTransformMatrix(bone);
}

void Model::updateBonesTransformMatrix(Bone &bone) {
    if (bone.hasParent()) { // 有父级
        // 子物体的global矩阵 = 父物体的global矩阵 * 子物体的local
        const Bone &parent = *bone.parent;
        bone.global = parent.global * bone.local;
    }
    else {
        if (!has_parent)
            bone.global = bone.local;
    }

    // 递归更新children
    if (bone.hasChildren()) {
        for (Bone *child : bone.children) {
            updateBonesTransformMatrix(*child);
        }
    }
}

Thetas Model::GetInitThetas() {
    Thetas theta = Thetas(DofsSize(), 0.0f);
    int idx = 0;
    for (const Dof &dof : dofs) {
        theta[idx] = dof.init_val;
        ++idx;
    }
    return theta;
}

// 更新每个center的位置
// 若该center是bone的root，则直接赋值global
// 若该center是bone的attachment，则使用公式，带入之前的offset计算
void Model::UpdateCentersPosition() {
    for (Bone &bone : bones) {
        if (bone.hasParent()) {
            smodel::vec3 global_pos = bone.global.block(0, 3, 3, 1);
            bone.center->position = global_pos;

            for (Sphere *center : bone.attachments) {
                if (center->isAttachment()) {
                    center->position = global_pos + bone.global.block(0, 0, 3, 3) * center->offset;
                }
            }
        }
    }
    ReindexBlocksByCenterRadii();
    ComputeTangentPoints();
}

void Model::ReindexBlocksByCenterRadii() {
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i][2] == BLOCK_UNDEFINED) { // 两个center构成的骨骼，第idx为0的center的半径要大于第1个
            if (centers[blocks[i][0]].radius < centers[blocks[i][1]].radius) {
                std::swap(blocks[i][0], blocks[i][1]);
            }
        }
        else {
            const Center &c0 = centers[blocks[i][0]], &c1 = centers[blocks[i][1]], &c2 = centers[blocks[i][2]];
            if (c0.radius <= c1.radius && c1.radius <= c2.radius) {
                blocks[i] = Block(blocks[i][2], blocks[i][1], blocks[i][0]);
            }
            if (c0.radius <= c2.radius && c2.radius <= c1.radius) {
                blocks[i] = Block(blocks[i][1], blocks[i][2], blocks[i][0]);
            }
            if (c1.radius <= c0.radius && c0.radius <= c2.radius) {
                blocks[i] = Block(blocks[i][2], blocks[i][0], blocks[i][1]);
            }
            if (c1.radius <= c2.radius && c2.radius <= c0.radius) {
                blocks[i] = Block(blocks[i][0], blocks[i][2], blocks[i][1]);
            }
            if (c2.radius <= c0.radius && c0.radius <= c1.radius) {
                blocks[i] = Block(blocks[i][1], blocks[i][0], blocks[i][2]);
            }
        }
    }
}

void Model::ComputeTangentPoints() {
    //cout << "centers.size() = " << centers.size() << endl;
    //cout << "radii.size() = " << radii.size() << endl;
    //cout << "blocks.size() = " << blocks.size() << endl;
    tangent_points = TangentPoints(blocks.size(), Tangent());
    for (int i = 0; i < blocks.size(); ++i) {
        if (blocks[i][2] == BLOCK_UNDEFINED) continue; // 两个center的block不计算
        //cout << "c1 = " << centers[blocks[i][0]][0] << endl;
        //cout << "c2 = " << centers[blocks[i][1]][0] << endl;
        //cout << "c3 = " << centers[blocks[i][2]][0] << endl;
        const Center &c1 = centers[blocks[i][0]], &c2 = centers[blocks[i][1]], &c3 = centers[blocks[i][2]];
        glm::vec3 c1_pos = glm::vec3(c1.position.x(), c1.position.y(), c1.position.z());
        glm::vec3 c2_pos = glm::vec3(c2.position.x(), c2.position.y(), c2.position.z());
        glm::vec3 c3_pos = glm::vec3(c3.position.x(), c3.position.y(), c3.position.z());
        computeTangentPoint(camera_ray,
                            c1_pos, c2_pos, c3_pos,
                            c1.radius, c2.radius, c3.radius,
                            tangent_points[i].v1, tangent_points[i].v2, tangent_points[i].v3,
                            tangent_points[i].u1, tangent_points[i].u2, tangent_points[i].u3,
                            tangent_points[i].n, tangent_points[i].m);
        //std::cout << "in model: (" << tangent_points[i].v1[0] << ", " << tangent_points[i].v1[1] << ", " << tangent_points[i].v1[2] << " ); " << std::endl;
    }
}

void Model::computeTangentPoint(const glm::vec3 &camera_ray,
                                glm::vec3 &c1, const glm::vec3 &c2, const glm::vec3 &c3,
                                float r1, float r2, float r3,
                                glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &v3,
                                glm::vec3 &u1, glm::vec3 &u2, glm::vec3 &u3,
                                glm::vec3 &n, glm::vec3 &m)
{

    /*std::cout << "c1 = (" << c1[0] << ", " << c1[1] << ", " << c1[2] << ")" << std::endl;
    std::cout << "c2 = (" << c2[0] << ", " << c2[1] << ", " << c2[2] << ")" << std::endl;
    std::cout << "c3 = (" << c3[0] << ", " << c3[1] << ", " << c3[2] << ")" << std::endl;
    std::cout << "r1 = " << r1 << std::endl;
    std::cout << "r2 = " << r2 << std::endl;
    std::cout << "r3 = " << r3 << std::endl;*/

    double epsilon = 1e-2;
    if (r1 - r2 < epsilon && r1 - r3 < epsilon) { // 三个球差不多大
        n = cross(c1 - c2, c1 - c3);
        n = n / length(n);
        if (dot(camera_ray, n) < 0) {
            m = -n;
        }
        else {
            m = n;
            n = -m;
        }
        v1 = c1 + r1 * n;
        v2 = c2 + r2 * n;
        v3 = c3 + r3 * n;
        u1 = c1 + r1 * m;
        u2 = c2 + r2 * m;
        u3 = c3 + r3 * m;
        /*std::cout << "n = (" << n[0] << ", " << n[1] << ", " << n[2] << ")" << std::endl;
        std::cout << "v1 = (" << v1[0] << ", " << v1[1] << ", " << v1[2] << ")" << std::endl;
        std::cout << "v2 = (" << v2[0] << ", " << v2[1] << ", " << v2[2] << ")" << std::endl;
        std::cout << "v3 = (" << v3[0] << ", " << v3[1] << ", " << v3[2] << ")" << std::endl;
        std::cout << std::endl << std::endl;*/
        return;
    }

    glm::vec3 z12 = c1 + (c2 - c1) * r1 / (r1 - r2);
    glm::vec3 z13 = c1 + (c3 - c1) * r1 / (r1 - r3);

    glm::vec3 l = (z12 - z13) / length(z12 - z13);
    float projection = dot(c1 - z12, l);
    glm::vec3 z = z12 + projection * l;

    float eta = length(c1 - z);
    float sin_beta = r1 / eta;
    float nu = sqrt(eta * eta - r1 * r1);
    float cos_beta = nu / eta;

    glm::vec3 f = (c1 - z) / eta;
    glm::vec3 h = cross(l, f);
    normalize(h);

    glm::vec3 g;

    g = sin_beta * h + cos_beta * f;
    v1 = z + nu * g;
    n = (v1 - c1) / length(v1 - c1);
    v2 = c2 + r2 * n;
    v3 = c3 + r3 * n;

    g = -sin_beta * h + cos_beta * f;
    u1 = z + nu * g;
    m = (u1 - c1) / length(u1 - c1);
    u2 = c2 + r2 * m;
    u3 = c3 + r3 * m;

    if (dot(camera_ray, n) > 0) {
        std::swap(v1, u1);
        std::swap(v2, u2);
        std::swap(v3, u3);
        std::swap(n, m);
    }
}

void Model::Resize(float uniform_scaling_factor, float width_scaling_factor, float thickness_scaling_factor) {
    Eigen::Matrix3f scaling_matrix = Eigen::Matrix3f::Identity();
    scaling_matrix(0, 0) = uniform_scaling_factor * width_scaling_factor;
    scaling_matrix(1, 1) = uniform_scaling_factor;
    scaling_matrix(2, 2) = uniform_scaling_factor;

    for (Sphere &center : centers) {
        center.radius *= uniform_scaling_factor * thickness_scaling_factor;
    }

    for (Bone &bone : bones) {
        bone.init_local.col(3).segment(0, 3) = scaling_matrix.cast<float>() * bone.init_local.col(3).segment(0, 3);
        for (Sphere *attachment : bone.attachments) {
            attachment->offset = scaling_matrix * attachment->offset;
        }
    }
}

void Model::PrintModel() {
    //std::cout << "CENTERS" << std::endl;
    //for (int i = 0; i < centers.size(); i++) {
    //    for (int j = 0; j < d; j++) {
    //        std::cout << centers[i][j] << " ";
    //    }
    //    std::cout << std::endl;
    //}
    //std::cout << "RADII" << std::endl;
    //for (int i = 0; i < radii.size(); i++) {
    //    std::cout << radii[i] << std::endl;
    //}
    //std::cout << "BLOCKS" << std::endl;
    //for (int i = 0; i < blocks.size(); i++) {
    //    for (int j = 0; j < d; j++) {
    //        if (blocks[i][j] < centers.size())
    //            std::cout << blocks[i][j] << " ";
    //    }
    //    std::cout << std::endl;
    //}
    //std::cout << "TANGENT POINTS" << std::endl;
    //for (int i = 0; i < tangent_points.size(); i++) {
    //    if (blocks[i][2] > centers.size()) continue;
    //    std::cout << "block " << i << std::endl;
    //    std::cout << "  v1: "; for (int j = 0; j < d; j++) std::cout << tangent_points[i].v1[j] << " "; std::cout << std::endl;
    //    std::cout << "  v2: "; for (int j = 0; j < d; j++) std::cout << tangent_points[i].v2[j] << " "; std::cout << std::endl;
    //    std::cout << "  v3: "; for (int j = 0; j < d; j++) std::cout << tangent_points[i].v3[j] << " "; std::cout << std::endl;
    //    std::cout << "  u1: "; for (int j = 0; j < d; j++) std::cout << tangent_points[i].u1[j] << " "; std::cout << std::endl;
    //    std::cout << "  u2: "; for (int j = 0; j < d; j++) std::cout << tangent_points[i].u2[j] << " "; std::cout << std::endl;
    //    std::cout << "  u3: "; for (int j = 0; j < d; j++) std::cout << tangent_points[i].u3[j] << " "; std::cout << std::endl;
    //}
}

void Model::WriteModel(std::string data_path, int frame_number) {
    //std::ofstream centers_file;
    //std::string centers_file_path = frame_number < 0 ? data_path + "C.txt" : data_path + "C-" + std::to_string(frame_number) + ".txt";
    //centers_file.open(centers_file_path, std::ios::ate | std::ios::out);
    //if (centers_file.is_open()) {
    //    centers_file << centers.size() << std::endl;
    //    for (size_t i = 0; i < centers.size(); i++) {
    //        for (size_t j = 0; j < 3; j++) {
    //            centers_file << centers[i][j] << " ";
    //        }
    //    }
    //}
    //else {
    //    centers_file.close();
    //    throw centers_file_path;
    //}
    //centers_file.close();

    //std::ofstream radii_file;
    //std::string radii_file_path = frame_number < 0 ? data_path + "R.txt" : data_path + "R-" + std::to_string(frame_number) + ".txt";
    //radii_file.open(radii_file_path, std::ios::ate | std::ios::out);
    //if (radii_file.is_open()) {
    //    radii_file << radii.size() << std::endl;
    //    for (size_t i = 0; i < radii.size(); i++) {
    //        radii_file << radii[i] << " ";
    //    }
    //}
    //else {
    //    radii_file.close();
    //    throw radii_file_path;
    //}
    //radii_file.close();

    //std::ofstream blocks_file;
    //std::string blocks_file_path = frame_number < 0 ? data_path + "B.txt" : data_path + "B-" + std::to_string(frame_number) + ".txt";
    //blocks_file.open(blocks_file_path, std::ios::ate | std::ios::out);
    //if (blocks_file.is_open()) {
    //    blocks_file << blocks.size() << std::endl;
    //    for (size_t i = 0; i < blocks.size(); i++) {
    //        for (size_t j = 0; j < 3; j++) {
    //            blocks_file << blocks[i][j] << " ";
    //        }
    //    }
    //}
    //else {
    //    blocks_file.close();
    //    throw blocks_file_path;
    //}
    //blocks_file.close();

    //std::ofstream theta_file;
    //std::string theta_file_path = frame_number < 0 ? data_path + "T.txt" : data_path + "T-" + std::to_string(frame_number) + ".txt";
    //theta_file.open(theta_file_path, std::ios::ate | std::ios::out);
    //if (theta_file.is_open()) {
    //    theta_file << theta.size() << std::endl;
    //    for (size_t i = 0; i < theta.size(); i++) {
    //        theta_file << theta[i] << " ";
    //    }
    //}
    //else {
    //    theta_file.close();
    //    throw theta_file_path;
    //}
    //theta_file.close();

    //std::ofstream transformations_file;
    //std::string transformations_file_path = frame_number < 0 ? data_path + "I.txt" : data_path + "I-" + std::to_string(frame_number) + ".txt";
    //transformations_file.open(transformations_file_path, std::ios::ate | std::ios::out);
    //if (transformations_file.is_open()) {
    //    transformations_file << bones.size() << std::endl;
    //    for (size_t i = 0; i < bones.size(); i++) {
    //        for (size_t u = 0; u < 4; u++) {
    //            for (size_t v = 0; v < 4; v++) {
    //                //transformations_file << bones[i].global(u, v) << " ";
    //                transformations_file << bones[i].init_local(v, u) << " ";
    //            }
    //        }
    //    }
    //}
    //else {
    //    transformations_file.close();
    //    throw transformations_file_path;
    //}
    //transformations_file.close();
}
