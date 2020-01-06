#ifndef CONVOLUTION_RENDERER_FINGER_MASK_H
#define CONVOLUTION_RENDERER_FINGER_MASK_H

#include "ConvolutionRendererMask.h"

namespace ui {

class ConvolutionRendererFingerMask : public ConvolutionRendererMask {
public:
    ConvolutionRendererFingerMask();
    ConvolutionRendererFingerMask(const std::string project_path, const smodel::ModelCtrl *model);
    ~ConvolutionRendererFingerMask();

    // Mask渲染相关
    void PassMaskToShader() override;

private:
    void SpecialInit();
};

}; // namespace ui

#endif // CONVOLUTION_RENDERER_FINGER_MASK_H
