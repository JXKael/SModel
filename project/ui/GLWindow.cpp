#include "GLWindow.h"
#include "QUIManager.h"

#include <iostream>
#include "direct.h"
#include <io.h>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

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

// OpenGL

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


// Render Images
void GLWindow::SaveScreenImg(const std::string &file_name) {
    QImage img = grabFramebuffer();
    img.save(file_name.c_str(), 0, 100);
}

void GLWindow::SaveScreenImgBatch(const std::string &file_path, const std::string &file_name, const std::string &file_suffix) {
    smodel::ModelCtrl *body_model = QUIManager::Instance().GetModel(BODY);
    if (nullptr != body_model) {
        float yaw = 0.0f;   // 0~360
        float pitch = 0.0f; // -89~89

        const std::string flle_folder = file_path + file_name + "-RGB/";
        _mkdir(flle_folder.c_str());
        int idx = 1;
        float yaw_stride = 360.0f / 32.0f;
        float pitch_stride = 178.0f / 32.0f;
        for (float yaw = 0.0f; yaw <= 360.0f; yaw += yaw_stride) {
            for (float pitch = 89.0f; pitch >= -89.0f; pitch -= pitch_stride) {
                camera.SetEulerAngles(yaw, pitch);
                camera.UpdateFocusCameraVectors();
                this->update();
                SaveScreenImg(flle_folder + file_name + "-" + QString("%1").arg(idx++).toStdString() + "." + file_suffix);
            }
        }
    }
}

void GLWindow::SetRenderHandScreen() {
    smodel::ModelCtrl *left_hand_model = QUIManager::Instance().GetModel(LEFT_HAND);
    smodel::ModelCtrl *right_hand_model = QUIManager::Instance().GetModel(RIGHT_HAND);
    if (nullptr != left_hand_model && nullptr != right_hand_model) {
        model_center = kModelCenter_RenderHand;
        camera_center = kCameraCenter_RenderHand;
        camera.Reset(camera_center, model_center);
        this->resize(800, 600);
        this->update();
    }
}

void GLWindow::mousePressEvent(QMouseEvent *event) {
    cursor_pos.x = event->x();
    cursor_pos.y = event->y();

    is_mouse_pressing = true;
}

void GLWindow::mouseReleaseEvent(QMouseEvent *event) {
    camera.SetBasePosition();
    camera.SetBaseEulerAngles();

    is_mouse_pressing = false;
}

void GLWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    // alt + double LB
    if (QApplication::keyboardModifiers() == Qt::AltModifier && event->button() == Qt::LeftButton) {
        camera_center = kCameraCenter;
        model_center = kModelCenter;
        camera.Reset(camera_center, model_center);
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
            camera.ProcessFocusPerspective(xoffset, yoffset);
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
            camera.ProcessEyePerspective(xoffset, yoffset);
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
    camera.SetBasePosition();
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
    //case Qt::Key_4:
    //    this->ProcessImage();
    //    break;
    case Qt::Key_Up:
        camera.ProcessMovement(kUDLR, glm::vec2(0, 100));
        break;
    case Qt::Key_9:
        QUIManager::Instance().ShowAnimPannel();
        break;
    default:
        break;
    }
    this->update();
}

void GLWindow::keyReleaseEvent(QKeyEvent *event) {

}

std::vector<std::string> listFiles(const char *dir) {
    std::vector<std::string> names;
    char dirNew[200];
    strcpy(dirNew, dir);
    strcat(dirNew, "\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索

    intptr_t handle;
    _finddata_t findData;

    handle = _findfirst(dirNew, &findData);
    if (handle == -1)        // 检查是否成功
        return names;

    do
    {
        if (findData.attrib & _A_SUBDIR) {
            if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
                continue;

            std::cout << findData.name << "\t<dir>\n";

            // 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
            strcpy(dirNew, dir);
            strcat(dirNew, "\\");
            strcat(dirNew, findData.name);

            listFiles(dirNew);
        }
        else {
            names.push_back(findData.name);
            // std::cout << findData.name << "\t" << findData.size << " bytes.\n";
        }
    } while (_findnext(handle, &findData) == 0);

    _findclose(handle);    // 关闭搜索句柄
    return names;
}

void GLWindow::ProcessImage() {
    const std::string path = "E:/Code/python/PytorchPlay/data/SingleHand/RGBImages/";
    const std::string dst_path = "E:/Code/python/PytorchPlay/data/SingleHand/Masks/";
    std::vector<std::string> all_files = listFiles(path.c_str());
    int idx = 0;
    for (const std::string &file_name : all_files) {
        std::cout << "Porcessing No." << idx << ": " << file_name << std::endl;
        cv::Mat rgb_image = cv::imread(path + file_name);
        int rows = rgb_image.rows;
        int cols = rgb_image.cols;
        cv::Mat mask_img(rows, cols, CV_8UC1);
        for (int j = 0; j < rows; ++j) {
            // uchar* outData = outImage.ptr<uchar>(k);
            for (int i = 0; i < cols ; ++i)
            {
                const cv::Vec3b &pixel = rgb_image.at<cv::Vec3b>(j, i);
                if (pixel[0] >= 245 && pixel[1] >= 245 && pixel[2] >= 245) {
                    mask_img.at<uchar>(j, i) = 0;
                }
                else {
                    mask_img.at<uchar>(j, i) = 4;
                }
            }
        }
        std::string out_name = file_name.substr(0, file_name.find_last_of(".")) + ".png";
        cv::imwrite(dst_path + out_name, mask_img);
        ++idx;
    }

}
