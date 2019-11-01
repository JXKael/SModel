#include "CenterAxisRenderer.h"

using namespace ui;

CenterAxisRenderer::CenterAxisRenderer() : GLRenderer() {
    SpecialInit();
}

CenterAxisRenderer::CenterAxisRenderer(const QString &project_path, const std::map<std::string, smodel::ModelCtrl *> &models)
  : GLRenderer(project_path + "/ui/shaders/axis_vs.glsl", project_path + "/ui/shaders/axis_fs.glsl"),
    models_(models)
{
    SpecialInit();
}

CenterAxisRenderer::~CenterAxisRenderer() {
    models_.clear();
}

void CenterAxisRenderer::SpecialInit() {
    this->SetRenderMode(GL_LINES);
}

void CenterAxisRenderer::BindStaticDrawData() {}

void CenterAxisRenderer::BindDynamicDrawData() {
    bool success = false;
    vao.bind();
    success = vbo.bind();
    assert(success);
    vbo.setUsagePattern(QGLBuffer::UsagePattern::DynamicDraw);

    success = shader_program.bind();
    assert(success);

    shader_program.release();
    vao.release();
    vbo.release();
}

void CenterAxisRenderer::Draw() {
    glDrawArrays(render_mode, 0, (GLsizei)(vertices.size() / 6));
}

void CenterAxisRenderer::PassVerticesToShader() {
    // 顶点
    shader_program.setAttributeBuffer("position", GL_FLOAT, 0, 3, 6 * sizeof(float));
    shader_program.enableAttributeArray("position");
}

void CenterAxisRenderer::PassVertexColorToShader() {
    // 颜色
    shader_program.setAttributeBuffer("color", GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float));
    shader_program.enableAttributeArray("color");
}

void CenterAxisRenderer::PassTexcoordToShader() {}

void CenterAxisRenderer::PassDynamicVerticesToShader() {
    this->vertices.clear();
    for (const std::pair<std::string, smodel::ModelCtrl *> model_pair : models_) {
        smodel::ModelCtrl *model = model_pair.second;
        const smodel::Centers &centers = model->GetCenters();
        int centerid = model->GetSelectedCenterid();
        if (centerid >= 0 && centerid < centers.size()) {
            const smodel::Center &center = centers[centerid];
            const smodel::mat4 &global = center.isAttachment() ? smodel::mat4::Zero() : center.bone->global;
            const smodel::mat4 &parent_global = center.isAttachment() ? center.attached_bone->global : center.bone->parent->global;

            Eigen::Vector3f o = parent_global.block(0, 3, 3, 1);
            glm::vec3 axis_o = glm::vec3(o.x(), o.y(), o.z());

            Eigen::Vector3f x = parent_global.block(0, 0, 3, 1);
            glm::vec3 axis_x = glm::vec3(x.x(), x.y(), x.z());

            Eigen::Vector3f y = parent_global.block(0, 1, 3, 1);
            glm::vec3 axis_y = glm::vec3(y.x(), y.y(), y.z());

            Eigen::Vector3f z = parent_global.block(0, 2, 3, 1);
            glm::vec3 axis_z = glm::vec3(z.x(), z.y(), z.z());
            convertVecToVerticesTransform(axis_o, axis_x, axis_y, axis_z, 120.0f);
        }
    }

    if (vertices.empty()) return;

    vbo.bind();

    vbo.allocate(vertices.data(), sizeof(float) * (int)vertices.size());
    this->PassVerticesToShader();
    this->PassVertexColorToShader();
    vbo.release();
}

void CenterAxisRenderer::convertVecToVerticesTransform(const glm::vec3 &axis_o, const glm::vec3 &axis_x, const glm::vec3 &axis_y, const glm::vec3 &axis_z, const float &ratio)
{
    float ox = axis_o.x;
    float oy = axis_o.y;
    float oz = axis_o.z;
    // 原点 & 颜色
    vertices.push_back(ox); vertices.push_back(oy); vertices.push_back(oz);
    vertices.push_back(red.x); vertices.push_back(red.y); vertices.push_back(red.z);
    // x轴 & 颜色
    vertices.push_back((ox + axis_x.x * ratio)); vertices.push_back((oy + axis_x.y * ratio)); vertices.push_back((oz + axis_x.z * ratio));
    vertices.push_back(red.x); vertices.push_back(red.y); vertices.push_back(red.z);

    // 原点 & 颜色
    vertices.push_back(ox); vertices.push_back(oy); vertices.push_back(oz);
    vertices.push_back(green.x); vertices.push_back(green.y); vertices.push_back(green.z);
    // y轴 & 颜色
    vertices.push_back((ox + axis_y.x * ratio)); vertices.push_back((oy + axis_y.y * ratio)); vertices.push_back((oz + axis_y.z * ratio));
    vertices.push_back(green.x); vertices.push_back(green.y); vertices.push_back(green.z);

    // 原点 & 颜色
    vertices.push_back(ox); vertices.push_back(oy); vertices.push_back(oz);
    vertices.push_back(blue.x); vertices.push_back(blue.y); vertices.push_back(blue.z);
    // z轴 & 颜色
    vertices.push_back((ox + axis_z.x * ratio)); vertices.push_back((oy + axis_z.y * ratio)); vertices.push_back((oz + axis_z.z * ratio));
    vertices.push_back(blue.x); vertices.push_back(blue.y); vertices.push_back(blue.z);
}
