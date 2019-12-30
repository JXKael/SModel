#include "Simple3DRenderer.h"

using namespace ui;

Simple3DRenderer::Simple3DRenderer(): GLRenderer() {
    SpecialInit();
}

Simple3DRenderer::Simple3DRenderer(const QString &vertex_shader, const QString &frag_shader)
  : GLRenderer(vertex_shader, frag_shader)
{
    SpecialInit();
}

Simple3DRenderer::~Simple3DRenderer() {}

void Simple3DRenderer::SpecialInit() {
    std::vector<float> axis_vertices = {
        // vertex                 // color
        0.0f,   0.0f,   0.0f,     1.0f, 0.0f, 0.0f,
        200.0f, 0.0f,   0.0f,     1.0f, 0.0f, 0.0f,

        0.0f,   0.0f,   0.0f,     0.0f, 1.0f, 0.0f,
        0.0f,   200.0f, 0.0f,     0.0f, 1.0f, 0.0f,

        0.0f,   0.0f,   0.0f,     0.0f, 0.0f, 1.0f,
        0.0f,   0.0f,   200.0f,   0.0f, 0.0f, 1.0f
    };

    this->SetRenderMode(GL_POINTS);
    // this->SetVertices(axis_vertices);
}

void Simple3DRenderer::Draw() {
    glDrawArrays(render_mode, 0, (GLsizei)(vertices.size() / 6));
}

void Simple3DRenderer::PassVerticesToShader() {
    // 顶点
    shader_program.setAttributeBuffer("position", GL_FLOAT, 0, 3, 6 * sizeof(float));
    shader_program.enableAttributeArray("position");
}

void Simple3DRenderer::PassVertexColorToShader() {
    // 颜色
    shader_program.setAttributeBuffer("color", GL_FLOAT, 3 * sizeof(float), 3, 6 * sizeof(float));
    shader_program.enableAttributeArray("color");
}

void Simple3DRenderer::PassTexcoordToShader() {}
