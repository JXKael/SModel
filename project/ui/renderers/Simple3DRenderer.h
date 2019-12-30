#ifndef SIMPLE_3D_RENDERER_H
#define SIMPLE_3D_RENDERER_H

#include "GLRenderer.h"

namespace ui {

class Simple3DRenderer : public GLRenderer {
public:
    Simple3DRenderer();
    Simple3DRenderer(const QString &vertex_shader, const QString &frag_shader);
    ~Simple3DRenderer();

    void Draw() override;

    void PassVerticesToShader();
    void PassVertexColorToShader();
    void PassTexcoordToShader();

private:
    void SpecialInit();
};

}; // namespace ui

#endif // SIMPLE_3D_RENDERER_H
