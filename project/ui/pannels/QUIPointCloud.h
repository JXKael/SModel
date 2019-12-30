#ifndef QUI_POINT_CLOUD_H
#define QUI_POINT_CLOUD_H

#include "Camera.h"
#include "renderers/Simple3DRenderer.h"

#include <QApplication>
#include <QOpenGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>

#include <glm/glm.hpp>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
//#include "opencv2/opencv.hpp"

#include <iostream>
#include <string>

namespace ui {

class QUIPointCloud : public QOpenGLWidget {
    Q_OBJECT

private:
    glm::vec3 model_center;
    glm::vec3 camera_center;

    std::string project_path_;
    std::string depth_img_path; // 深度图路径
    cv::Mat depth_img; // 深度图
    std::vector<glm::vec3> point_cloud; // 点云列表

    Camera camera;

    GLRenderer *gl_renderer;


    bool is_file_path_right = false;
    bool isReadSuccess = false;

    float minDistance = 10000.0f;
    float maxDistance = 0.0f;

    bool is_mouse_pressing;
    glm::uvec2 cursor_pos;
public:
    explicit QUIPointCloud(QWidget *parent = 0);
    explicit QUIPointCloud(const std::string &project_path, QWidget *parent = 0);
    ~QUIPointCloud();

public:
    void Init();
    void SetMinMax(float /*min*/, float /*max*/);
    void SetDepthImgPath(const std::string &/*depthImgPath*/);
    inline bool IsReadSuccess() { return this->isReadSuccess; }

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

private:
    void ReadDepthImg();
    void PassToRenderer();
};

}

#endif // QUI_POINT_CLOUD_H
