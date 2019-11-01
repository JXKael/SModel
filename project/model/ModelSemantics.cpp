#include "ModelSemantics.h"

using namespace smodel;

ModelSemantics::ModelSemantics(Model &model)
  : model_(model),
    centers_(model_.centers),
    bones_(model_.bones),
    dofs_(model_.dofs)
{

}

ModelSemantics::~ModelSemantics() {}

void ModelSemantics::Init() {
    this->InitTopology();

    model_.ReindexBlocksByCenterRadii();
    model_.ComputeTangentPoints();

    this->InitBonesTransformMatrix();
    this->InitAttachmentCentersOffset();

    model_.MoveToInit();
}

void ModelSemantics::InitTopology() {
    this->InitCenters();
    this->InitBones();
    this->InitDofs();
}

void ModelSemantics::InitCenters() {
    // 根据bone中的attachment_ids初始化center中的attached_bone
    for (Bone &bone : bones_) {
        if (bone.hasAttachments()) {
            for (const int &centerid : bone.attachment_ids) {
                if (centerid >= 0 && centerid < centers_.size()) {
                    centers_[centerid].attached_bone = &bone;
                    if (centers_[centerid].type != SphereType::kAttachment) {
                        std::cout << "!!! centerid: " << centerid << " 配置为非attachment，却在" << bone.id << "中配置为attachment" << std::endl;
                    }
                }
            }
        }
    }
    // 校验
    for (const Sphere &center : centers_) {
        if (center.isAttachment() && center.attached_bone == nullptr) {
            std::cout << "!!! centerid: " << center.id << " 配置为attachment，却未在Bones.csv中配置" << std::endl;
        }
    }
}

void ModelSemantics::InitBones() {
    for (Bone &bone : bones_) {
        if (bone.center_id >= 0 && bone.center_id < centers_.size()) {
            bone.center = &centers_[bone.center_id];
            centers_[bone.center_id].bone = &bone;
        }
        if (bone.parent_id >= 0 && bone.parent_id < bones_.size()) {
            bone.parent = &bones_[bone.parent_id];
            bone.parent->children.push_back(&bone);
        }
        if (bone.hasAttachments()) {
            for (const int &attachment_id : bone.attachment_ids) {
                if (attachment_id >= 0 && attachment_id < centers_.size()) {
                    bone.attachments.push_back(&centers_[attachment_id]);
                }
            }
        }
    }

    for (Bone &bone : bones_) {
        if (nullptr != bone.parent && nullptr != bone.parent->center) {
            bone.radius_parent = bone.parent->center->radius;
        }
        if (nullptr != bone.center) {
            bone.radius_child = bone.center->radius;
        }
        if (nullptr != bone.parent && nullptr != bone.parent->center && nullptr != bone.center) {
            bone.length = (bone.center->position - bone.parent->center->position).norm();
        }
    }
}

void ModelSemantics::InitDofs() {
    for (Dof &dof : dofs_) {
        if (dof.bone_id >= 0 && dof.bone_id < bones_.size()) {
            dof.bone = &bones_[dof.bone_id];
        }
    }
    model_.theta.resize(dofs_.size());
}

void ModelSemantics::InitBonesTransformMatrix() {
    // model_.MoveToInit();
    model_.Move(Thetas(model_.DofsSize(), 0));
}

void ModelSemantics::InitAttachmentCentersOffset() {
    for (Center &center : centers_) {
        if (center.isAttachment()) {
            smodel::vec3 bone_position = center.attached_bone->global.block(0, 3, 3, 1);
            center.offset = center.attached_bone->global.block(0, 0, 3, 3).inverse() * (center.position - bone_position);
        }
    }
}

