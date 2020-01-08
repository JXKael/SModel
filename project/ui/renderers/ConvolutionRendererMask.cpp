#include "ConvolutionRendererMask.h"

using namespace ui;

ConvolutionRendererMask::ConvolutionRendererMask() : ConvolutionRenderer() {
    SpecialInit();
}

ConvolutionRendererMask::ConvolutionRendererMask(const std::string project_path, const smodel::ModelCtrl *model)
  : ConvolutionRenderer(project_path, model)
{
    SpecialInit();
}

ConvolutionRendererMask::~ConvolutionRendererMask() {}

void ConvolutionRendererMask::SpecialInit() {
    std::vector<float> convolution_vertices = {
        // vertex
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f
    };

    this->vertex_shader_ = QString((project_path_+ "/ui/shaders/model_mask_vs.glsl").c_str());
    this->frag_shader_ = QString((project_path_ + "/ui/shaders/model_mask_fs.glsl").c_str());

    this->SetRenderMode(GL_TRIANGLE_STRIP);
    this->SetVertices(convolution_vertices);
}

void ConvolutionRendererMask::Draw() {
    glDrawArrays(render_mode, 0, (GLsizei)(vertices.size() / 3));
}

void ConvolutionRendererMask::PassVerticesToShader() {
    // 顶点
    shader_program.setAttributeBuffer("position", GL_FLOAT, 0, 3, 3 * sizeof(float));
    shader_program.enableAttributeArray("position");
}

void ConvolutionRendererMask::PassVertexColorToShader() {}

void ConvolutionRendererMask::PassTexcoordToShader() {}

void ConvolutionRendererMask::PassOtherToShader() {
    if (shader_program.isLinked()) {
        PassWindowToShader();
        PassRenderModelToShader();
        PassMaskToShader();
    }
}

void ConvolutionRendererMask::PassMaskToShader() {
    int mask_val = model_->GetMaskValue();
    shader_program.setUniformValue("mask_color", (GLfloat)((float)mask_val / 255.0f));
}
