#ifndef FIX_AXIS_RENDERER_H
#define FIX_AXIS_RENDERER_H

#include "GLRenderer.h"
#include "model/ModelCtrl.h"

namespace ui {

class FixAxisRenderer : public GLRenderer {
public:
    FixAxisRenderer();
    FixAxisRenderer(const QString &vertex_shader, const QString &frag_shader);
    ~FixAxisRenderer();

    void Draw() override;

    void PassVerticesToShader() override;
    void PassVertexColorToShader() override;
    void PassTexcoordToShader() override;
private:
    void SpecialInit();
};

}; // namespace ui

#endif // FIX_AXIS_RENDERER_H
