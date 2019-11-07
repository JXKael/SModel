#include "FixAxisRenderer.h"

using namespace ui;

FixAxisRenderer::FixAxisRenderer() : GLRenderer() {
    SpecialInit();
}

FixAxisRenderer::FixAxisRenderer(const QString &vertex_shader, const QString &frag_shader)
  : GLRenderer(vertex_shader, frag_shader)
{
    SpecialInit();
}

FixAxisRenderer::~FixAxisRenderer() {}

void FixAxisRenderer::SpecialInit() {
    std::vector<float> axis_vertices = {
        // vertex                 // color
        0.0f,   0.0f,   0.0f,     1.0f, 0.0f, 0.0f,
        200.0f, 0.0f,   0.0f,     1.0f, 0.0f, 0.0f,

        0.0f,   0.0f,   0.0f,     0.0f, 1.0f, 0.0f,
        0.0f,   200.0f, 0.0f,     0.0f, 1.0f, 0.0f,

        0.0f,   0.0f,   0.0f,     0.0f, 0.0f, 1.0f,
        0.0f,   0.0f,   200.0f,   0.0f, 0.0f, 1.0f
    };

    this->SetRenderMode(GL_LINES);
    this->SetVertices(axis_vertices);
}

void FixAxisRenderer::Draw() {
    glDrawArrays(render_mode, 0, (GLsizei)(vertices.size() / 6));
}

void FixAxisRenderer::PassVerticesToShader() {
    // 顶点
    shader_program.setAttributeBuffer("position", GL_FLOAT, 0, 3, 6 * sizeof(float));
    shader_program.enableAttributeArray("position");
}

void FixAxisRenderer::PassVertexColorToShader() {
    // 颜色
    shader_program.setAttributeBuffer("color", GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float));
    shader_program.enableAttributeArray("color");
}

void FixAxisRenderer::PassTexcoordToShader() {}
