#ifndef GLWINDWO_H
#define GLWINDWO_H

#include <assert.h>

#include <QApplication>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>

#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model/ModelCtrl.h"

#include "OpenGL/ConvolutionRenderer.h"
#include "OpenGL/FixAxisRenderer.h"

#include "pannels/QUIPannelCtrl.h"

namespace ui {

#define M_PI 3.14159265358979323846

class GLWindow : public QOpenGLWidget
{
    Q_OBJECT

private:
    glm::vec3 model_center;
    glm::vec3 camera_center;
    Camera camera;

    std::vector<GLRenderer *> renderers;
    std::map<std::string, smodel::ModelCtrl *> models;

    bool is_mouse_pressing;
    glm::uvec2 cursor_pos;

    QUIPannelCtrl *pannel_ctrl_;

public:
    GLWindow(QWidget *parent = nullptr);
    ~GLWindow();

    inline bool IsMousePressing() { return is_mouse_pressing; }
    inline void AddModel(const std::string name, smodel::ModelCtrl &amodel) { models[name] = &amodel; }
    inline void AddRenderer(GLRenderer &arenderer) { renderers.push_back(&arenderer); }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

} // namespace ui

#endif // GLWINDWO_H
