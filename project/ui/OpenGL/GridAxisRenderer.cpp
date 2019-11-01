#include "GridAxisRenderer.h"

using namespace ui;

GridAxisRenderer::GridAxisRenderer() : GLRenderer() {
    SpecialInit();
}

GridAxisRenderer::GridAxisRenderer(const QString &project_path)
  : GLRenderer(project_path + "/ui/shaders/gridAxis_vs.glsl", project_path + "/ui/shaders/gridAxis_fs.glsl") {
    SpecialInit();
}

GridAxisRenderer::~GridAxisRenderer() {}

void GridAxisRenderer::SpecialInit() {
    for (float x = -far_point; x <= far_point; x += step) {
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(-far_point);

        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(far_point);
    }

    for (float z = -far_point; z <= far_point; z += step) {
        vertices.push_back(-far_point);
        vertices.push_back(0.0f);
        vertices.push_back(z);

        vertices.push_back(far_point);
        vertices.push_back(0.0f);
        vertices.push_back(z);
    }

    this->SetRenderMode(GL_LINES);
}

void GridAxisRenderer::PaintGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    vao.bind();
    shader_program.bind();

    if (!vertices.empty()) {
        this->PassMVPToShader();

        Draw();
    }
    shader_program.release();
    vao.release();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void GridAxisRenderer::Draw() {
    glDrawArrays(render_mode, 0, (GLsizei)(vertices.size() / 3));
}

void GridAxisRenderer::PassVerticesToShader() {
    // 顶点
    shader_program.setAttributeBuffer("position", GL_FLOAT, 0, 3, 3 * sizeof(float));
    shader_program.enableAttributeArray("position");
}

void GridAxisRenderer::PassVertexColorToShader() {
    // 颜色
    shader_program.setUniformValue("color", QVector4D(color.r, color.g, color.b, color.a));
}

void GridAxisRenderer::PassTexcoordToShader() {}
