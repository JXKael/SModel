#ifndef CONVOLUTION_RENDERER_MASK_H
#define CONVOLUTION_RENDERER_MASK_H

#include "ConvolutionRenderer.h"

namespace ui {

class ConvolutionRendererMask : public ConvolutionRenderer {
public:
    ConvolutionRendererMask();
    ConvolutionRendererMask(const std::string project_path, const smodel::ModelCtrl *model);
    ~ConvolutionRendererMask();

    void Draw() override;

    void PassVerticesToShader() override;
    void PassVertexColorToShader() override;
    void PassTexcoordToShader() override;
    void PassOtherToShader() override;

    // Mask渲染相关
    virtual void PassMaskToShader();

private:
    void SpecialInit();
};

}; // namespace ui

#endif // CONVOLUTION_RENDERER_MASK_H
