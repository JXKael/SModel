#ifndef GRID_AXIS_RENDERER_H
#define GRID_AXIS_RENDERER_H

#include "GLRenderer.h"
#include "model/ModelCtrl.h"

namespace ui {

class GridAxisRenderer : public GLRenderer {
private:
    const float far_point = 3000.0f;
    const float step = 100.0f;
    const glm::vec4 color = glm::vec4(0.3f, 0.3f, 0.3f, 0.1f);
public:
    GridAxisRenderer();
    GridAxisRenderer(const QString &project_path);
    ~GridAxisRenderer();

    void PaintGL() override;
    void Draw() override;

    void PassVerticesToShader() override;
    void PassVertexColorToShader() override;
    void PassTexcoordToShader() override;
private:
    void SpecialInit();
}; // class GridAxisRenderer

}; // namespace ui

#endif // GRID_AXIS_RENDERER_H
