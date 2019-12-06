#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#include <assert.h>

#include <QApplication>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "QUIConst.h"

#include "Camera.h"
#include "renderers/ConvolutionRenderer.h"
#include "renderers/FixAxisRenderer.h"
#include "renderers/CenterAxisRenderer.h"
#include "renderers/GridAxisRenderer.h"
#include "renderers/ConvolutionRendererMask.h"

#define M_PI 3.14159265358979323846
#define GL_WINDOW_WIDTH 1600
#define GL_WINDOW_HEIGHT 900

namespace ui {

const glm::vec3 kModelCenter = glm::vec3(0, 0, 0);
const glm::vec3 kCameraCenter = glm::vec3(0, 100, 1200);
const glm::vec3 kModelCenter_RenderHand = glm::vec3(0, 0, 123);
const glm::vec3 kCameraCenter_RenderHand = glm::vec3(0, 0, 600);

class GLWindow : public QOpenGLWidget {
    Q_OBJECT

private:
    glm::vec3 model_center;
    glm::vec3 camera_center;
    Camera camera;

    renderers_map renderers;
    renderers_state_map renderers_state;

    bool is_mouse_pressing;
    glm::uvec2 cursor_pos;

    std::string project_path_;

public:
    GLWindow(QWidget *parent = nullptr);
    ~GLWindow();

    void SetupRenderers(models_map &models);
    void ClearRenderer();
    void SetRendererState(const int &id, const bool &is_render);
    inline renderers_map &GetRenderers() { return renderers; }
    inline renderers_state_map &GetRenderersState() { return renderers_state; }

    inline bool IsMousePressing() { return is_mouse_pressing; }
    inline void SetProjectPath(const std::string &project_path) { this->project_path_ = project_path; }

    void SaveScreenImg(const std::string &file_name);
    void SaveScreenImgBatch(const std::string &file_path, const std::string &file_name, const std::string &file_suffix);
    void SaveScreenImgMask(const std::string &file_name);
    void SaveScreenImgMaskBatch(const std::string &file_path, const std::string &file_name, const std::string &file_suffix);

    void SetRenderHandScreen();
    void ResetScreen();
    void SetMaskVal(const std::string &model_name, const int &mask_val);

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
    // void keyReleaseEvent(QKeyEvent *event) override;

    void ProcessImage();
}; // class GLWindow

} // namespace ui

#endif // GL_WINDOW_H
