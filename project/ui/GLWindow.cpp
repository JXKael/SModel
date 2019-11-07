#include "GLWindow.h"
#include "QUIManager.h"
#include <iostream>

using namespace ui;

GLWindow::GLWindow(QWidget *parent)
  : QOpenGLWidget(parent),
    model_center(kModelCenter),
    camera_center(kCameraCenter),
    camera(camera_center, model_center),
    cursor_pos(0, 0)
{
    is_mouse_pressing = false;
}

GLWindow::~GLWindow() {
    makeCurrent();
    ClearRenderer();
    doneCurrent();
}

void GLWindow::SetupRenderers(models_map &models) {
    this->ClearRenderer();
    // 模型渲染
    int idx = 0;
    for (models_map::iterator it = models.begin(); it != models.end(); ++it) {
        renderers[idx] = std::make_shared<ui::ConvolutionRenderer>(project_path_, it->second);
        renderers[idx]->SetName(it->second->GetName());
        renderers_state[idx] = true;
        ++idx;
    }

    // 坐标网格渲染
    renderers[150] = std::make_shared<ui::GridAxisRenderer>(project_path_.c_str());
    renderers[150]->SetName("grid");
    renderers_state[150] = true;

    // center轴渲染
    renderers[3] = std::make_shared<ui::CenterAxisRenderer>(project_path_.c_str(), models);
    renderers[3]->SetName("select mark");
    renderers_state[3] = true;
}

void GLWindow::ClearRenderer() {
    renderers.clear();
}

void GLWindow::SetRendererState(const int &id, const bool &is_render) {
    std::map<int, bool>::iterator it = renderers_state.find(id);
    if (it != renderers_state.end()) {
        it->second = is_render;
    }
}

void GLWindow::initializeGL() {
    for (renderers_map::iterator it = renderers.begin(); it != renderers.end(); ++it) {
        it->second->InitializeGL();
    }

    std::cout << "--> OpenGL 版本: " << glGetString(GL_VERSION) << std::endl;
}

void GLWindow::resizeGL(int w, int h) {
    for (renderers_map::iterator it = renderers.begin(); it != renderers.end(); ++it) {
        it->second->ResizeGL(w, h);
    }
}

void GLWindow::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.fovy), (float)this->width() / (float)this->height(), camera.near_plane, camera.far_plane);

    for (renderers_map::iterator it = renderers.begin(); it != renderers.end(); ++it) {
        if (renderers_state[it->first]) {
            GLRenderer *renderer = it->second.get();
            renderer->SetCamera(this->camera);
            renderer->SetModel(model);
            renderer->SetView(view);
            renderer->SetProjection(projection);

            renderer->PaintGL();
        }
    }
}

void GLWindow::mousePressEvent(QMouseEvent *event) {
    cursor_pos.x = event->x();
    cursor_pos.y = event->y();

    is_mouse_pressing = true;
}

void GLWindow::mouseReleaseEvent(QMouseEvent *event) {
    camera.SetBasePosition();
    camera.SetBaseDirection();

    is_mouse_pressing = false;
}

void GLWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    // alt + double LB
    if (QApplication::keyboardModifiers() == Qt::AltModifier && event->button() == Qt::LeftButton) {
        camera_center = kCameraCenter;
        model_center = kModelCenter;
        camera.Reset(camera_center, model_center, camera.GetCameraType());
    }
    this->update();
}

void GLWindow::mouseMoveEvent(QMouseEvent *event) {
    float xoffset = (float)event->x() - (float)cursor_pos.x;
    float yoffset = (float)event->y() - (float)cursor_pos.y;
    // std::cout << xoffset << ", " << yoffset << std::endl;
    if (QApplication::keyboardModifiers() == Qt::AltModifier) {
        if (event->buttons() == Qt::LeftButton) {
            // 聚焦在一个视点上旋转
            camera.ProcessPerspective(xoffset, yoffset);
        }
        else if (event->buttons() == Qt::MiddleButton) {
            // 上下左右平移
            camera.ProcessMovement(kUDLR, glm::vec2(xoffset, yoffset));
        }
        else if (event->buttons() == Qt::RightButton) {
            // 前进&后退
            camera.ProcessMovement(kFB, glm::vec2(xoffset, yoffset));
        }
    }
    else {
        if (event->buttons() == Qt::MiddleButton) {
            // 上下左右平移
            camera.ProcessMovement(kUDLR, glm::vec2(xoffset, yoffset));
        }
        else if (event->buttons() == Qt::RightButton) {
            // 视角旋转
            camera.ProcessPerspective(xoffset, yoffset);
        }
    }
    this->update();
}

void GLWindow::wheelEvent(QWheelEvent *event) {
    // std::cout << event->angleDelta().x() << std::endl;
    if (event->orientation() == Qt::Horizontal)
        camera.ProcessScroll((float)event->angleDelta().x());
    else if (event->orientation() == Qt::Vertical) 
        camera.ProcessScroll((float)event->angleDelta().y());
    this->update();
}

void GLWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_0:
        QUIManager::Instance().ShowQuickPannel();
        break;
    case Qt::Key_1:
        QUIManager::Instance().ShowBodyPannel();
        break;
    case Qt::Key_2:
        QUIManager::Instance().ShowRightHandPannel();
        break;
    case Qt::Key_3:
        QUIManager::Instance().ShowLeftHandPannel();
        break;
    case Qt::Key_9:
        QUIManager::Instance().ShowAnimPannel();
        break;
    case Qt::Key_Alt:
        this->camera.SetCameraType(CameraType::kFocus);
        this->camera.UpdateCameraVectors();
        break;
    default:
        break;
    }
    this->update();
}

void GLWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Alt:
        this->camera.SetCameraType(CameraType::kFree);
        this->camera.UpdateCameraVectors();
        break;
    default:
        break;
    }
    this->update();
}
