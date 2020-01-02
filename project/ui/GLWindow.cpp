#include "GLWindow.h"
#include "QUIManager.h"
#include "utils/dir_utils.h"

#include <iostream>
#include <iomanip>
#include "direct.h"
#include <io.h>

//#include "opencv2/core.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace ui;

cv::Mat QImage2cvMat(QImage &image, bool clone = true, bool rb_swap = false);
cv::Mat convertToMaskImg(cv::Mat &rgba_img);

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
    // mask渲染
    for (models_map::iterator it = models.begin(); it != models.end(); ++it) {
        renderers[idx] = std::make_shared<ui::ConvolutionRendererMask>(project_path_, it->second);
        renderers[idx]->SetName(it->second->GetName() + "_mask");
        renderers_state[idx] = false;
        ++idx;
    }
    // finger_mask渲染
    for (models_map::iterator it = models.begin(); it != models.end(); ++it) {
        renderers[idx] = std::make_shared<ui::ConvolutionRendererFingerMask>(project_path_, it->second);
        renderers[idx]->SetName(it->second->GetName() + "_finger_mask");
        renderers_state[idx] = false;
        ++idx;
    }

    // center轴渲染
    renderers[idx] = std::make_shared<ui::CenterAxisRenderer>(project_path_.c_str(), models);
    renderers[idx]->SetName("select mark");
    renderers_state[idx] = true;

    // 坐标网格渲染
    renderers[150] = std::make_shared<ui::GridAxisRenderer>(project_path_.c_str());
    renderers[150]->SetName("grid");
    renderers_state[150] = true;
}

void GLWindow::ClearRenderer() {
    renderers.clear();
}

void GLWindow::SetRendererState(const int &id, const bool &is_render) {
    renderers_state_map::iterator it = renderers_state.find(id);
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
    cv::Mat rgba_image = QImage2cvMat(grabFramebuffer());
    cv::imwrite(file_name, rgba_image);
}

void GLWindow::SaveScreenImgMask(const std::string &file_name) {
    cv::Mat rgba_image = QImage2cvMat(grabFramebuffer());
    cv::imwrite(file_name, convertToMaskImg(rgba_image));
}

void GLWindow::SaveScreenImgBatch(const std::string &file_path, const std::string &file_name, const std::string &batch_name, const std::string &file_suffix, const bool &is_mask) {
    smodel::ModelCtrl *body_model = QUIManager::Instance().GetModel(BODY);
    if (nullptr != body_model) {
        float yaw = 0.0f;   // 0~360
        float pitch = 0.0f; // -89~89

        const std::string flle_folder = file_path + file_name + "-" + batch_name + "/";
        _mkdir(flle_folder.c_str());
        int idx = 1;
        float yaw_stride = 360.0f / 32.0f;
        float pitch_stride = 178.0f / 32.0f;
        std::cout << "--> Starting rendering [" << batch_name << "] batch of " << file_name << ":" << std::endl;
        for (float yaw = 0.0f; yaw <= 360.0f; yaw += yaw_stride) {
            for (float pitch = 89.0f; pitch >= -89.0f; pitch -= pitch_stride) {
                camera.SetEulerAngles(yaw, pitch);
                camera.UpdateFocusCameraVectors();
                this->update();
                const std::string file_name_c = QString("%1-%2-%3.%4").arg(file_name.c_str()).arg(idx++).arg(batch_name.c_str()).arg(file_suffix.c_str()).toStdString();
                std::cout << "\r... Rendering........" << file_name_c;

                if (is_mask) {
                    this->SaveScreenImgMask(flle_folder + file_name_c);
                } else {
                    this->SaveScreenImg(flle_folder + file_name_c);
                }
            }
        }
        std::cout << "\r--> Rendering over !                                                       " << std::endl;
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

void GLWindow::ResetScreen() {
    camera_center = kCameraCenter;
    model_center = kModelCenter;
    camera.Reset(camera_center, model_center);
    this->resize(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT);
    this->update();
}

// event

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
    case Qt::Key_4:
        this->ProcessImage();
        break;
    //case Qt::Key_Up:
    //    camera.ProcessMovement(kUDLR, glm::vec2(0, 100));
    //    break;
    case Qt::Key_9:
        QUIManager::Instance().ShowAnimPannel();
        break;
    default:
        break;
    }
    this->update();
}


// other function

cv::Mat get_kernel(int kern_size, int white_val, unsigned int &th_b, unsigned int &th_t) {
    cv::Mat kernel(kern_size, kern_size, CV_8UC1);
    unsigned int th_bottom = 0;
    unsigned int th_top = 0;
    int i = 1;
    for (int k_row = 1; k_row < kern_size - 1; ++k_row) {
        for (int k_col = 1; k_col < kern_size - 1; ++k_col) {
            th_top += (1 * white_val);
            kernel.at<cv::uint8_t>(k_row, k_col) = 1;
            i++;
        }
    }
    for (int k_col = 0; k_col < kern_size - 1; ++k_col) {
        th_bottom += (i * white_val);
        kernel.at<cv::uint8_t>(0, k_col) = i++;
    }
    for (int k_row = 0; k_row < kern_size - 1; ++k_row) {
        th_bottom += (i * white_val);
        kernel.at<cv::uint8_t>(k_row, kern_size - 1) = i++;
    }
    for (int k_col = kern_size - 1; k_col > 0; --k_col) {
        th_bottom += (i * white_val);
        kernel.at<cv::uint8_t>(kern_size - 1, k_col) = i++;
    }
    for (int k_row = kern_size - 1; k_row > 0; --k_row) {
        th_bottom += (i * white_val);
        kernel.at<cv::uint8_t>(k_row, 0) = i++;
    }
    th_top += th_bottom;
    //for (int k_row = 0; k_row < kern_size; ++k_row) {
    //    for (int k_col = 0; k_col < kern_size; ++k_col) {
    //        int val = (int)kernel.at<cv::uint8_t>(k_row, k_col);
    //        std::cout << std::setw(2) << val << " ";
    //    }
    //    std::cout << std::endl;
    //}
    th_b = th_bottom;
    th_t = th_top;
    return kernel;
}

int get_seg_line(cv::Mat &image_l) {
    int rows = image_l.rows;
    int cols = image_l.cols;
    int seg = ceil(6.5f / 10.0f * (float)rows);
    for (int col = seg; col > 0; --col) {
        bool q = true;
        for (int row = 0; row < rows; ++row) {
            const cv::uint8_t &pixel = image_l.at<cv::uint8_t>(row, col);
            if (pixel > 0) {
                q = false;
                break;
            }
        }
        if (q) {
            seg = col;
            break;
        }
    }
    return seg;
}

void GLWindow::ProcessImage() {
    std::string depth_img_src = "E:/Datasets/SignData_30/Original/output_A/depth_0.png";
    std::string color_img_src = "E:/Datasets/SignData_30/Original/output_A/color_0.png";
    std::string dst_img_path = color_img_src.substr(0, 42) + "color_0_ppp.png";
    cv::Mat depth_img = cv::imread(depth_img_src, -1);
    cv::Mat color_img = cv::imread(color_img_src, -1);
    int rows = depth_img.rows;
    int cols = depth_img.cols;
    cv::uint16_t min = 1 << 15;
    cv::uint16_t max = 0;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            cv::uint16_t &pixel = depth_img.at<cv::uint16_t>(row, col);
            if (pixel > 500) {
                cv::Vec3b &color_pixel = color_img.at<cv::Vec3b>(row, col);
                color_pixel[0] = 255;
                color_pixel[1] = 255;
                color_pixel[2] = 255;
            }
        }
    }
    cv::imwrite(dst_img_path, color_img);
}

cv::Mat QImage2cvMat(QImage &image, bool clone, bool rb_swap)
{
    cv::Mat mat;
    //qDebug() << image.format();
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void *)image.constBits(), image.bytesPerLine());
        if (clone)  mat = mat.clone();
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void *)image.constBits(), image.bytesPerLine());
        if (clone)  mat = mat.clone();
        // if (rb_swap) cv::cvtColor(mat, mat, cv::CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void *)image.bits(), image.bytesPerLine());
        if (clone)  mat = mat.clone();
        break;
    }
    return mat;
}

cv::Mat convertToMaskImg(cv::Mat &rgba_img) {
    int rows = rgba_img.rows;
    int cols = rgba_img.cols;
    cv::Mat mask_img(rows, cols, CV_8UC1);
    for (int j = 0; j < rows; ++j) {
        for (int i = 0; i < cols; ++i)
        {
            const cv::Vec4b &pixel = rgba_img.at<cv::Vec4b>(j, i);
            if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255)
                mask_img.at<uchar>(j, i) = 0; // 背景为黑色
            else if (pixel[0] == pixel[1] && pixel[0] == pixel[2])
                mask_img.at<uchar>(j, i) = pixel[0];
            else
                mask_img.at<uchar>(j, i) = 0;
        }
    }
    return mask_img;
}
