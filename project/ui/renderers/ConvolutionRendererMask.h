#ifndef CONVOLUTION_RENDERER_MASK_H
#define CONVOLUTION_RENDERER_MASK_H

#include "GLRenderer.h"
#include "model/ModelCtrl.h"

namespace ui {

class ConvolutionRendererMask : public GLRenderer {
private:
    struct Light {
        glm::vec3 Ia = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Id = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 Is = glm::vec3(1, 1, 1);
        glm::vec3 position = glm::vec3(0.0f, 100.0f, 100.0f);
    } light;

    struct Material {
        glm::vec3 ka = 0.65f * glm::vec3(0.9176, 0.7412, 0.6157);
        glm::vec3 kd = 0.75f * glm::vec3(0.9176, 0.7412, 0.6157);
        glm::vec3 ks = glm::vec3(0, 0, 0);
        float p = 60.0f;
    } material;

    const smodel::ModelCtrl *model_;

    std::string project_path_;

    GLint window_left = 0;
    GLint window_bottom = 0;
    GLint window_width = 0;
    GLint window_height = 0;

    int mask_val_;
public:
    ConvolutionRendererMask();
    ConvolutionRendererMask(const std::string project_path, const smodel::ModelCtrl *model, const int mask_val = 0);
    ~ConvolutionRendererMask();
    void SetMaskVal(const int &mask_val);
    void ResizeGL(int w, int h) override;

    void Draw() override;

    void PassVerticesToShader() override;
    void PassVertexColorToShader() override;
    void PassTexcoordToShader() override;
    void PassMVPToShader() override;
    void PassOtherToShader() override;
    // Convolution渲染相关
    void PassWindowToShader();
    void PassRenderModelToShader();

private:
    void SpecialInit();
};

}; // namespace ui

#endif // CONVOLUTION_RENDERER_MASK_H
