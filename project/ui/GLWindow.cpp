#include "GLWindow.h"
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
    pannel_ctrl = new QUIPannelCtrl(this);
}

GLWindow::~GLWindow() {
    makeCurrent();
    models.clear();
    for (const std::pair<int, GLRenderer *> &renderer_pair : renderers) {
        if (nullptr != renderer_pair.second)
            delete renderer_pair.second;
    }
    renderers.clear();

    if (nullptr != pannel_ctrl)
        delete pannel_ctrl;
    pannel_ctrl = nullptr;

    doneCurrent();
}

void GLWindow::SetupRenderers() {
    // 模型渲染
    int idx = 0;
    for (const std::pair<std::string, smodel::ModelCtrl *> &model_pair : models) {
        const smodel::ModelCtrl *model = model_pair.second;
        ConvolutionRenderer *con_renderer = new ConvolutionRenderer(project_path_, model);
        AddRenderer(idx, con_renderer);
        ++idx;
    }

    // 坐标网格渲染
    GridAxisRenderer *grid_renderer = new GridAxisRenderer(project_path_.c_str());
    AddRenderer(150, grid_renderer);

    // center轴渲染
    CenterAxisRenderer *center_renderer = new CenterAxisRenderer(project_path_.c_str(), models);
    AddRenderer(3, center_renderer);
}

void GLWindow::initializeGL() {
    SetupRenderers();
    for (const std::pair<int, GLRenderer *> &renderer_pair : renderers) {
        renderer_pair.second->InitializeGL();
    }

    std::cout << "--> OpenGL 版本: " << glGetString(GL_VERSION) << std::endl;
}

void GLWindow::resizeGL(int w, int h) {
    for (const std::pair<int, GLRenderer *> &renderer_pair : renderers) {
        renderer_pair.second->ResizeGL(w, h);
    }
}

void GLWindow::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.fovy), (float)this->width() / (float)this->height(), camera.near_plane, camera.far_plane);

    for (const std::pair<int, GLRenderer *> &renderer_pair : renderers) {
        GLRenderer *renderer = renderer_pair.second;
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
        this->pannel_ctrl->ShowAnimPannel(this->models);
        break;
    case Qt::Key_1:
        this->pannel_ctrl->ShowBodyPannel(this->models["body"]);
        break;
    case Qt::Key_2:
        this->pannel_ctrl->ShowRightHandPannel(this->models["RightHand"]);
        break;
    case Qt::Key_3:
        this->pannel_ctrl->ShowLeftHandPannel(this->models["LeftHand"]);
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

