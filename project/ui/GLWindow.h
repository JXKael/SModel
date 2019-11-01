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
#include "OpenGL/CenterAxisRenderer.h"
#include "OpenGL/GridAxisRenderer.h"

#include "pannels/QUIPannelCtrl.h"

namespace ui {

#define M_PI 3.14159265358979323846

const glm::vec3 kModelCenter = glm::vec3(0, 0, 0);
const glm::vec3 kCameraCenter = glm::vec3(0, 20, 1200);

class GLWindow : public QOpenGLWidget {
    Q_OBJECT

private:
    glm::vec3 model_center;
    glm::vec3 camera_center;
    Camera camera;

    std::map<int, GLRenderer *> renderers;
    std::map<std::string, smodel::ModelCtrl *> models;

    bool is_mouse_pressing;
    glm::uvec2 cursor_pos;

    QUIPannelCtrl *pannel_ctrl;

    std::string project_path_;

public:
    GLWindow(QWidget *parent = nullptr);
    ~GLWindow();

    void SetupRenderers();
    inline bool IsMousePressing() { return is_mouse_pressing; }
    inline void AddModel(const std::string name, smodel::ModelCtrl &model) { models[name] = &model; }
    inline void AddRenderer(const int &order, GLRenderer *renderer) { renderers[order] = renderer; }
    inline void SetProjectPath(const std::string &project_path) { this->project_path_ = project_path; }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
}; // class GLWindow

} // namespace ui

#endif // GLWINDWO_H
