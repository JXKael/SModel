#ifndef CENTER_AXIS_RENDERER_H
#define CENTER_AXIS_RENDERER_H

#include "GLRenderer.h"
#include "model/ModelCtrl.h"

namespace ui {

class CenterAxisRenderer : public GLRenderer {
private:
    const glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
    std::map<std::string, smodel::ModelCtrl *> models_;
public:
    CenterAxisRenderer();
    CenterAxisRenderer(const QString &project_path, const std::map<std::string, smodel::ModelCtrl *> &models);
    ~CenterAxisRenderer();

    void BindStaticDrawData() override;
    void BindDynamicDrawData() override;

    void Draw() override;

    void PassVerticesToShader() override;
    void PassVertexColorToShader() override;
    void PassTexcoordToShader() override;

    void PassDynamicVerticesToShader() override;
private:
    void SpecialInit();
    void convertVecToVerticesTransform(const glm::vec3 &axis_o, const glm::vec3 &axis_x, const glm::vec3 &axis_y, const glm::vec3 &axis_z, const float &ratio);
}; // class CenterAxisRenderer

}; // namespace ui

#endif // CENTER_AXIS_RENDERER_H
