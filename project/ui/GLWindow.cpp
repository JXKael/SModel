#include "GLWindow.h"
#include <iostream>

using namespace ui;

GLWindow::GLWindow(QWidget *parent)
  : QOpenGLWidget(parent),
    model_center(0, 0, 0),
    camera_center(0, 0, 1200),
    camera(camera_center, model_center),
    cursor_pos(0, 0)
{
    is_mouse_pressing = false;
    pannel_ctrl_ = new QUIPannelCtrl(this);
}

GLWindow::~GLWindow() {
    models.clear();
    renderers.clear();
}

void GLWindow::initializeGL() {
    for (GLRenderer *renderer : renderers) {
        renderer->InitializeGL();
    }

    std::cout << "--> OpenGL 版本: " << glGetString(GL_VERSION) << std::endl;
}

void GLWindow::resizeGL(int w, int h) {
    for (GLRenderer *renderer : renderers) {
        renderer->ResizeGL(w, h);
    }
}

void GLWindow::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.fovy), (float)this->width() / (float)this->height(), camera.near_plane, camera.far_plane);

    for (GLRenderer *renderer : renderers) {
        renderer->SetCamera(this->camera);
        renderer->SetModel(model);
        renderer->SetView(view);
        renderer->SetProjection(projection);

        renderer->PaintGL();
    }
}

void GLWindow::mousePressEvent(QMouseEvent *event) {
    cursor_pos.x = event->x();
    cursor_pos.y = event->y();
    is_mouse_pressing = true;
}

void GLWindow::mouseReleaseEvent(QMouseEvent *event) {
    camera.SetBaseDirection();
    is_mouse_pressing = false;
}

void GLWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    // alt + LB
    if (QApplication::keyboardModifiers() == Qt::AltModifier && event->button() == Qt::LeftButton) {
        camera.Reset(camera_center, model_center, camera.GetCameraType());
    }
    this->update();
}

void GLWindow::mouseMoveEvent(QMouseEvent *event) {
    float xoffset = (float)event->x() - (float)cursor_pos.x;
    float yoffset = (float)event->y() - (float)cursor_pos.y;
    if (event->buttons() == Qt::LeftButton) {
        camera.ProcessPerspective(xoffset, yoffset);
    }
    this->update();
}

void GLWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_0:
        this->pannel_ctrl_->ShowBodyPannel(this->models["body"]);
        break;
    case Qt::Key_1:
        this->pannel_ctrl_->ShowRightHandPannel(this->models["RightHand"]);
        break;
    case Qt::Key_2:
        this->pannel_ctrl_->ShowLeftHandPannel(this->models["LeftHand"]);
        break;
    case Qt::Key_Alt:
        this->camera.SetCameraType(CameraType::kFocus);
        this->camera.UpdateCameraVectors();
        break;
    // 镜头漫游
    case Qt::Key_W:
        camera.ProcessMovement(CameraMovement::kForward);
        break;
    case Qt::Key_S:
        camera.ProcessMovement(CameraMovement::kBackward);
        break;
    case Qt::Key_A:
        camera.ProcessMovement(CameraMovement::kLeft);
        break;
    case Qt::Key_D:
        camera.ProcessMovement(CameraMovement::kRight);
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

