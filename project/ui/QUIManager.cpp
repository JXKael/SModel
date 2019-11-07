#include "QUIManager.h"

using namespace ui;

QUIManager::QUIManager() {
    gl_window = nullptr;
    ui_quick = nullptr;
    ui_body = nullptr;
    ui_right_hand = nullptr;
    ui_left_hand = nullptr;
    ui_anim = nullptr;
}

QUIManager::~QUIManager() {}

QUIManager &QUIManager::Instance() {
    static QUIManager instance;
    return instance;
}

void QUIManager::Init() {
    gl_window->SetupRenderers(models);
}

void QUIManager::Show() {
    this->gl_window->show();
}

void QUIManager::UpdateGL() {
    this->gl_window->update();
}

void QUIManager::SetSelected(const std::string &name, const int &center_id) {
    this->name = name;
    this->center_id = center_id;
}

const std::string &QUIManager::GetSelectedModel() const {
    return this->name;
}

const int &QUIManager::GetSelectedCenterId() const {
    return this->center_id;
}

void QUIManager::ShowQuickPannel() {
    if (nullptr == ui_quick) {
        ui_quick = new QUIQuick(project_path_, models, gl_window->GetRenderers());
        ui_quick->setParent(gl_window, Qt::WindowType::Dialog);
        ui_quick->Init();
    }
    if (nullptr != ui_quick) {
        bool isVisible = ui_quick->isVisible();
        if (!isVisible) {
            ui_quick->show();
        }
    }
}

void QUIManager::ShowBodyPannel() {
    models_map::iterator it = models.find(BODY);
    if (it != models.end()) {
        if (nullptr == ui_body) {
            ui_body = new QUIDashboard(it->second);
            ui_body->setParent(gl_window, Qt::WindowType::Dialog);
            ui_body->Init();
            ui_body->setWindowTitle("Body Pannel");
        }
        if (nullptr != ui_body) {
            bool isVisible = ui_body->isVisible();
            if (!isVisible) {
                ui_body->show();
            }
        }
    } else {
        std::cout << "no " << BODY << " model" << std::endl;
    }
}

void QUIManager::ShowRightHandPannel() {
    models_map::iterator it = models.find(RIGHT_HAND);
    if (it != models.end()) {
        if (nullptr == ui_right_hand) {
            ui_right_hand = new QUIDashboard(it->second);
            ui_right_hand->setParent(gl_window, Qt::WindowType::Dialog);
            ui_right_hand->Init();
            ui_right_hand->setWindowTitle("Right Hand Pannel");
        }
        if (nullptr != ui_right_hand) {
            bool isVisible = ui_right_hand->isVisible();
            if (!isVisible) {
                ui_right_hand->show();
            }
        }
    } else {
        std::cout << "no " << RIGHT_HAND << " model" << std::endl;
    }
}

void QUIManager::ShowLeftHandPannel() {
    models_map::iterator it = models.find(LEFT_HAND);
    if (it != models.end()) {
        if (nullptr == ui_left_hand) {
            ui_left_hand = new QUIDashboard(it->second);
            ui_left_hand->setParent(gl_window, Qt::WindowType::Dialog);
            ui_left_hand->Init();
            ui_left_hand->setWindowTitle("Left Hand Pannel");
        }
        if (nullptr != ui_left_hand) {
            bool isVisible = ui_left_hand->isVisible();
            if (!isVisible) {
                ui_left_hand->show();
            }
        }
    } else {
        std::cout << "no " << LEFT_HAND << " model" << std::endl;
    }
}

void QUIManager::ShowAnimPannel() {
    if (nullptr == ui_anim) {
        ui_anim = new QUIAnim(models);
        ui_anim->setParent(gl_window, Qt::WindowType::Dialog);
        ui_anim->Init();
    }
    if (nullptr != ui_anim) {
        bool isVisible = ui_anim->isVisible();
        if (!isVisible) {
            ui_anim->show();
        }
    }
}

void QUIManager::SetRendererState(const int &id, const bool &is_render) {
    if (nullptr != gl_window) {
        gl_window->SetRendererState(id, is_render);
    }
}
