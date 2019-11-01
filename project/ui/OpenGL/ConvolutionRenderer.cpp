#include "ConvolutionRenderer.h"

using namespace ui;

ConvolutionRenderer::ConvolutionRenderer() : GLRenderer() {
    SpecialInit();
}

ConvolutionRenderer::ConvolutionRenderer(const std::string project_path, const smodel::ModelCtrl *model)
  : project_path_(project_path),
    model_(model)
{
    SpecialInit();
}

ConvolutionRenderer::~ConvolutionRenderer() {}

void ConvolutionRenderer::SpecialInit() {
    std::vector<float> convolution_vertices = {
        // vertex              // texture coord
        -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,    1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,    0.0f, 1.0f,
         1.0f,  1.0f, 0.0f,    1.0f, 1.0f
    };
    
    this->vertex_shader_ = QString((project_path_+ "/ui/shaders/model_vs.glsl").c_str());
    this->frag_shader_ = QString((project_path_ + "/ui/shaders/model_fs.glsl").c_str());

    this->SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    this->SetRenderMode(GL_TRIANGLE_STRIP);
    this->SetVertices(convolution_vertices);
    this->AddTexture("synthetic_texture", (project_path_ + "/ui/shaders/skin_texture.png").c_str());
}

void ConvolutionRenderer::ResizeGL(int w, int h) {
    // 防止窗口大小变为0
    if (h == 0) h = 1;
    // 重置当前的视口
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    this->window_width = w;
    this->window_height = h;
}

void ConvolutionRenderer::Draw() {
    glDrawArrays(render_mode, 0, (GLsizei)(vertices.size() / 5));
}

void ConvolutionRenderer::PassVerticesToShader() {
    // 顶点
    shader_program.setAttributeBuffer("position", GL_FLOAT, 0, 3, 5 * sizeof(float));
    shader_program.enableAttributeArray("position");
}

void ConvolutionRenderer::PassVertexColorToShader() {}

void ConvolutionRenderer::PassTexcoordToShader() {
    // 纹理坐标
    shader_program.setAttributeBuffer("texcoord", GL_FLOAT, 3 * sizeof(float), 2, 5 * sizeof(float));
    shader_program.enableAttributeArray("texcoord");
}

void ConvolutionRenderer::PassMVPToShader() {
    glm::mat4 MVP = projection * view * model;

    shader_program.setUniformValue("camera_center", QVector3D(camera.position.x, camera.position.y, camera.position.z));
    shader_program.setUniformValue("MVP", QMatrix4x4(glm::value_ptr(MVP), 4, 4));
    shader_program.setUniformValue("invMVP", QMatrix4x4(glm::value_ptr(glm::inverse(MVP)), 4, 4));
    shader_program.setUniformValue("model", QMatrix4x4(glm::value_ptr(model), 4, 4));
    shader_program.setUniformValue("view", QMatrix4x4(glm::value_ptr(view), 4, 4));
    shader_program.setUniformValue("projection", QMatrix4x4(glm::value_ptr(projection), 4, 4));
}

void ConvolutionRenderer::PassOtherToShader() {
    if (shader_program.isLinked()) {
        PassWindowToShader();
        PassLightingToShader();
        PassMaterialToShader();
        PassRenderModelToShader();
    }
}

void ConvolutionRenderer::PassWindowToShader() {
    shader_program.setUniformValue("window_left", (GLint)this->window_left);
    shader_program.setUniformValue("window_bottom", (GLint)this->window_bottom);
    shader_program.setUniformValue("window_width", (GLint)this->window_width);
    shader_program.setUniformValue("window_height", (GLint)this->window_height);
}

void ConvolutionRenderer::PassLightingToShader() {
    shader_program.setUniformValue("light_pos", QVector3D(light.position.x, light.position.y, light.position.z));
    shader_program.setUniformValue("Ia", QVector3D(light.Ia.x, light.Ia.y, light.Ia.z));
    shader_program.setUniformValue("Id", QVector3D(light.Id.x, light.Id.y, light.Id.z));
    shader_program.setUniformValue("Is", QVector3D(light.Is.x, light.Is.y, light.Is.z));
}

void ConvolutionRenderer::PassMaterialToShader() {
    shader_program.setUniformValue("ka", QVector3D(material.ka.x, material.ka.y, material.ka.z));
    shader_program.setUniformValue("kd", QVector3D(material.kd.x, material.kd.y, material.kd.z));
    shader_program.setUniformValue("ks", QVector3D(material.ks.x, material.ks.y, material.ks.z));
    shader_program.setUniformValue("p", material.p);
}

void ConvolutionRenderer::PassRenderModelToShader() {
    const smodel::Blocks &blocks = model_->GetBlocks();
    const smodel::Centers &centers = model_->GetCenters();
    const smodel::TangentPoints &tangent_points = model_->GetTangentPoints();

    std::vector<glm::vec3> gl_centers;
    std::vector<GLfloat> gl_radii;
    std::vector<glm::ivec3> gl_blocks;

    for (const smodel::Center &center : centers) {
        gl_centers.push_back(glm::vec3(center.position.x(), center.position.y(), center.position.z()));
        gl_radii.push_back(center.radius);
    }

    for (const smodel::Block &block : blocks) {
        gl_blocks.push_back(glm::ivec3(block.x(), block.y(), block.z()));
    }

    size_t num_centers = gl_centers.size();
    size_t num_blocks = gl_blocks.size();
    size_t num_tangents_points = tangent_points.size();

    shader_program.setUniformValue("num_centers", (GLint)num_centers);
    shader_program.setUniformValue("num_blocks", (GLint)num_blocks);

    //shader_program.setUniformValueArray("centers", (GLfloat *)gl_centers.data(), num_centers, 3);
    //shader_program.setUniformValueArray("radii", (GLfloat *)gl_radii.data(), num_centers, 1);
    //shader_program.setUniformValueArray("blocks", (GLint *)gl_blocks.data(), num_blocks);

    glUniform3fv(glGetUniformLocation(shader_program.programId(), "centers"), (GLsizei)gl_centers.size(), (GLfloat *)gl_centers.data());
    glUniform1fv(glGetUniformLocation(shader_program.programId(), "radii"), (GLsizei)gl_radii.size(), (GLfloat *)gl_radii.data());
    glUniform3iv(glGetUniformLocation(shader_program.programId(), "blocks"), (GLsizei)gl_blocks.size(), (GLint *)gl_blocks.data());


    std::vector<glm::vec3> tangents_v1 = std::vector<glm::vec3>(num_tangents_points, glm::vec3(0, 0, 0));
    std::vector<glm::vec3> tangents_v2 = std::vector<glm::vec3>(num_tangents_points, glm::vec3(0, 0, 0));
    std::vector<glm::vec3> tangents_v3 = std::vector<glm::vec3>(num_tangents_points, glm::vec3(0, 0, 0));
    std::vector<glm::vec3> tangents_u1 = std::vector<glm::vec3>(num_tangents_points, glm::vec3(0, 0, 0));
    std::vector<glm::vec3> tangents_u2 = std::vector<glm::vec3>(num_tangents_points, glm::vec3(0, 0, 0));
    std::vector<glm::vec3> tangents_u3 = std::vector<glm::vec3>(num_tangents_points, glm::vec3(0, 0, 0));
    for (int i = 0; i < tangent_points.size(); i++) {
        tangents_v1[i] = tangent_points[i].v1;
        tangents_v2[i] = tangent_points[i].v2;
        tangents_v3[i] = tangent_points[i].v3;
        tangents_u1[i] = tangent_points[i].u1;
        tangents_u2[i] = tangent_points[i].u2;
        tangents_u3[i] = tangent_points[i].u3;
    }
    //shader_program.setUniformValueArray("tangents_v1", (GLfloat *)tangents_v1.data(), tangents_v1.size(), 3);
    //shader_program.setUniformValueArray("tangents_v2", (GLfloat *)tangents_v2.data(), tangents_v2.size(), 3);
    //shader_program.setUniformValueArray("tangents_v3", (GLfloat *)tangents_v3.data(), tangents_v3.size(), 3);
    //shader_program.setUniformValueArray("tangents_u1", (GLfloat *)tangents_u1.data(), tangents_u1.size(), 3);
    //shader_program.setUniformValueArray("tangents_u2", (GLfloat *)tangents_u2.data(), tangents_u2.size(), 3);
    //shader_program.setUniformValueArray("tangents_u3", (GLfloat *)tangents_u3.data(), tangents_u3.size(), 3);

    glUniform3fv(glGetUniformLocation(shader_program.programId(), "tangents_v1"), (GLsizei)tangents_v1.size(), (GLfloat *)tangents_v1.data());
    glUniform3fv(glGetUniformLocation(shader_program.programId(), "tangents_v2"), (GLsizei)tangents_v2.size(), (GLfloat *)tangents_v2.data());
    glUniform3fv(glGetUniformLocation(shader_program.programId(), "tangents_v3"), (GLsizei)tangents_v3.size(), (GLfloat *)tangents_v3.data());
    glUniform3fv(glGetUniformLocation(shader_program.programId(), "tangents_u1"), (GLsizei)tangents_u1.size(), (GLfloat *)tangents_u1.data());
    glUniform3fv(glGetUniformLocation(shader_program.programId(), "tangents_u2"), (GLsizei)tangents_u2.size(), (GLfloat *)tangents_u2.data());
    glUniform3fv(glGetUniformLocation(shader_program.programId(), "tangents_u3"), (GLsizei)tangents_u3.size(), (GLfloat *)tangents_u3.data());
}
