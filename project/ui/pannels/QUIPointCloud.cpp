#include "QUIPointCloud.h"

using namespace ui;

QUIPointCloud::QUIPointCloud(QWidget *parent)
  : QOpenGLWidget(parent),
    model_center(0, 0, 0),
    camera_center(0, 0, 500),
    gl_renderer(nullptr)
{
    this->resize(640, 480);
    this->move(20, 30);
}

QUIPointCloud::QUIPointCloud(const std::string &project_path, QWidget *parent)
  : QOpenGLWidget(parent),
    project_path_(project_path),
    model_center(0, 0, 0),
    camera_center(0, 0, 500),
    camera(camera_center, model_center)
{
    this->resize(640, 480);
    this->move(20, 30);
    QString vs_path = QString("%1").arg((project_path_ + "/ui/shaders/simple3D_vs.glsl").c_str());
    QString fs_path = QString("%1").arg((project_path_ + "/ui/shaders/simple3D_fs.glsl").c_str());
    gl_renderer = new Simple3DRenderer(vs_path, fs_path);
}

QUIPointCloud::~QUIPointCloud() {
    if (nullptr != gl_renderer) delete gl_renderer;
    point_cloud.clear();
}

void QUIPointCloud::Init() {
    this->ReadDepthImg();
    this->PassToRenderer();
}

void QUIPointCloud::initializeGL() {
    if (nullptr != gl_renderer)
        gl_renderer->InitializeGL();
}

void QUIPointCloud::resizeGL(int w, int h) {
    if (nullptr != gl_renderer)
        gl_renderer->ResizeGL(w, h);
}

void QUIPointCloud::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.fovy), (float)this->width() / (float)this->height(), camera.near_plane, camera.far_plane);

    if (nullptr != gl_renderer) {
        gl_renderer->SetCamera(this->camera);
        gl_renderer->SetModel(model);
        gl_renderer->SetView(view);
        gl_renderer->SetProjection(projection);

        gl_renderer->PaintGL();
    }
}

void QUIPointCloud::SetMinMax(float min, float max) {
    this->minDistance = min;
    this->maxDistance = max;
}

void QUIPointCloud::SetDepthImgPath(const std::string &depthImgPath) {
    this->is_file_path_right = false;
    this->depth_img_path = depthImgPath;

    auto findResult = depthImgPath.find(".png");
    is_file_path_right = (findResult != std::string::npos);
}

void QUIPointCloud::ReadDepthImg() {
    this->isReadSuccess = false;
    this->depth_img = cv::imread(depth_img_path, -1);

    int channels = depth_img.channels();
    int depth = depth_img.depth();
    isReadSuccess = this->is_file_path_right && depth_img.data != NULL && depth_img.rows > 0 && depth_img.cols > 0 && channels == 1 && depth == CV_16U;
    if (!isReadSuccess) return;

    int imgWidth = depth_img.cols;
    int imgHeight = depth_img.rows;

    const float fx = 800;
    const float fy = 800;
    const float cx = (float)imgWidth / 2.0f;
    const float cy = (float)imgHeight / 2.0f;

    this->resize(imgWidth, imgHeight);
    this->move(20, 30);

    point_cloud.clear();
    point_cloud.reserve(imgWidth * imgHeight);
    for (int i = 0; i < imgHeight; i++)  //行循环
    {
        cv::uint16_t *dstData = depth_img.ptr<cv::uint16_t>(i);
        for (int j = 0; j < imgWidth; j++)   //列循环
        {
            // 获得图像中存储的深度值
            unsigned short depthMeter = dstData[j];

            //if (depthMeter <= this->maxDistance && depthMeter >= this->minDistance) {
            if (depthMeter < 65535) {
                float depthMeter_f = (float)depthMeter;
                float x = ((float)j - cx) / fx * depthMeter_f;
                float y = -((float)i - cy) / fy * depthMeter_f;
                float z = -depthMeter_f;
                point_cloud.emplace_back(x, y, z);

                if (depthMeter_f < minDistance) minDistance = depthMeter_f;
                if (depthMeter_f > maxDistance) maxDistance = depthMeter_f;
            }
        }
    }
}

void QUIPointCloud::PassToRenderer() {
    if (point_cloud.size() <= 0) return;

    std::vector<float> vertices;
    for (const glm::vec3 point : point_cloud) {
        if (point[2] > -maxDistance) {
            vertices.push_back(point[0]);
            vertices.push_back(point[1]);
            vertices.push_back(point[2]);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    if (nullptr != gl_renderer) {
        gl_renderer->SetVertices(vertices);
    }
}

void QUIPointCloud::mousePressEvent(QMouseEvent *event) {
    cursor_pos.x = event->x();
    cursor_pos.y = event->y();

    is_mouse_pressing = true;
}

void QUIPointCloud::mouseReleaseEvent(QMouseEvent *event) {
    camera.SetBasePosition();
    camera.SetBaseEulerAngles();

    is_mouse_pressing = false;
}

void QUIPointCloud::mouseDoubleClickEvent(QMouseEvent *event) {
    // alt + double LB
    if (QApplication::keyboardModifiers() == Qt::AltModifier && event->button() == Qt::LeftButton) {
        model_center = glm::vec3(0, 0, 0);
        camera_center = glm::vec3(0, 0, 500);
        camera.Reset(camera_center, model_center);
    }
    this->update();
}

void QUIPointCloud::mouseMoveEvent(QMouseEvent *event) {
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

void QUIPointCloud::wheelEvent(QWheelEvent *event) {
    // std::cout << event->angleDelta().x() << std::endl;
    if (event->orientation() == Qt::Horizontal)
        camera.ProcessScroll((float)event->angleDelta().x());
    else if (event->orientation() == Qt::Vertical)
        camera.ProcessScroll((float)event->angleDelta().y());
    camera.SetBasePosition();
    this->update();
}

void QUIPointCloud::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        default: {
            break;
        }
    }
}
