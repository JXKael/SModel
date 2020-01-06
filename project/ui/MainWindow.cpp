#include "MainWindow.h"
#include "QUIManager.h"
#include "utils/dir_utils.h"

#include <iostream>
#include <iomanip>
#include "direct.h"
#include <io.h>

// #include "opencv2/opencv.hpp"

using namespace ui;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    this->move(0, 0);
}

MainWindow::~MainWindow() {
    // makeCurrent();

    // doneCurrent();
}

void MainWindow::Init() {
    this->central_widget = new QWidget(this);

    this->central_layout = new QGridLayout();
    central_layout->setSpacing(0);
    central_layout->setMargin(0);

    this->central_widget->setLayout(central_layout);

    QMenuBar *menu_bar = new QMenuBar();
    this->setMenuBar(menu_bar);
    // 文件
    QMenu *file_menu = new QMenu("File(&F)", menu_bar);
    menu_bar->addMenu(file_menu);
    // 编辑
    QMenu *edit_menu = new QMenu("Edit(&E)", menu_bar);
    menu_bar->addMenu(edit_menu);
    // 渲染
    QMenu *render_menu = new QMenu("Render(&R)", menu_bar);
    menu_bar->addMenu(render_menu);
    // 窗口
    QMenu *view_menu = new QMenu("View(&V)", menu_bar);
    view_menu->addAction("Animation", this, SLOT(onOpenAnimation()), QKeySequence("Ctrl+9"));
    menu_bar->addMenu(view_menu);
    // 帮助
    QMenu *help_menu = new QMenu("Help(&H)", menu_bar);
    menu_bar->addMenu(help_menu);

    this->setCentralWidget(central_widget);
}

void MainWindow::SetGLWidget(GLWidget *gl_widget) {
    this->gl_widget_ = gl_widget;
    this->central_layout->addWidget(gl_widget_, 1, 1, 1, 1);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
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
        //this->ProcessImage();
        //break;
        //case Qt::Key_Up:
        //    camera.ProcessMovement(kUDLR, glm::vec2(0, 100));
        //    break;
    //case Qt::Key_9:
    //    QUIManager::Instance().ShowAnimPannel();
    //    break;
    default:
        break;
    }
    this->update();
}

/// 槽函数

void MainWindow::onOpenAnimation() {
    QUIManager::Instance().ShowAnimPannel();
}

