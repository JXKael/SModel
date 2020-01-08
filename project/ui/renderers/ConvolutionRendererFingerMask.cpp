#include "ConvolutionRendererFingerMask.h"

using namespace ui;

ConvolutionRendererFingerMask::ConvolutionRendererFingerMask() : ConvolutionRendererMask() {
    SpecialInit();
}

ConvolutionRendererFingerMask::ConvolutionRendererFingerMask(const std::string project_path, const smodel::ModelCtrl *model)
  : ConvolutionRendererMask(project_path, model)
{
    SpecialInit();
}

ConvolutionRendererFingerMask::~ConvolutionRendererFingerMask() {}

void ConvolutionRendererFingerMask::SpecialInit() {
    std::vector<float> convolution_vertices = {
        // vertex
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f
    };

    this->vertex_shader_ = QString((project_path_+ "/ui/shaders/model_finger_mask_vs.glsl").c_str());
    this->frag_shader_ = QString((project_path_ + "/ui/shaders/model_finger_mask_fs.glsl").c_str());

    this->SetRenderMode(GL_TRIANGLE_STRIP);
    this->SetVertices(convolution_vertices);
}

void ConvolutionRendererFingerMask::PassMaskToShader() {
    const smodel::BlockColors &block_colors = model_->GetBlockColors();

    std::vector<GLfloat> qgl_mask_colors;

    for (const smodel::BlockColor &block_color : block_colors) {
        qgl_mask_colors.push_back(((GLfloat)block_color) / 255.0f);
    }

    shader_program.setUniformValueArray("mask_colors", qgl_mask_colors.data(), (int)qgl_mask_colors.size(), 1);
}
