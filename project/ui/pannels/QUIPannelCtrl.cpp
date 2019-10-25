#include "QUIPannelCtrl.h"
#include "GLWindow.h"

using namespace ui;

QUIPannelCtrl::QUIPannelCtrl(GLWindow *gl_window) : gl_window_(gl_window) {
    ui_body = nullptr;
    ui_right_hand = nullptr;
    ui_left_hand = nullptr;
}

QUIPannelCtrl::~QUIPannelCtrl() {
    delete ui_body;
    delete ui_right_hand;
    delete ui_left_hand;
}

void QUIPannelCtrl::UpdateGL() {
    this->gl_window_->update();
}

void QUIPannelCtrl::ShowBodyPannel(smodel::ModelCtrl *body_ctrl) {
    if (nullptr == ui_body) {
        ui_body = new QUIDashboard(this, body_ctrl);
        ui_body->setParent(gl_window_, Qt::WindowType::Dialog);
        ui_body->Init();
    }
    if (nullptr != ui_body) {
        bool isVisible = ui_body->isVisible();
        if (!isVisible) {
            ui_body->show();
        }
    }
}

void QUIPannelCtrl::ShowRightHandPannel(smodel::ModelCtrl *right_hand_ctrl) {
    if (nullptr == ui_right_hand) {
        ui_right_hand = new QUIDashboard(this, right_hand_ctrl);
        ui_right_hand->setParent(gl_window_, Qt::WindowType::Dialog);
        ui_right_hand->Init();
    }
    if (nullptr != ui_right_hand) {
        bool isVisible = ui_right_hand->isVisible();
        if (!isVisible) {
            ui_right_hand->show();
        }
    }
}

void QUIPannelCtrl::ShowLeftHandPannel(smodel::ModelCtrl *left_hand_ctrl) {
    if (nullptr == ui_left_hand) {
        ui_left_hand = new QUIDashboard(this, left_hand_ctrl);
        ui_left_hand->setParent(gl_window_, Qt::WindowType::Dialog);
        ui_left_hand->Init();
    }
    if (nullptr != ui_left_hand) {
        bool isVisible = ui_left_hand->isVisible();
        if (!isVisible) {
            ui_left_hand->show();
        }
    }
}
