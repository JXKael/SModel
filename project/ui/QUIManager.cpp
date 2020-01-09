#include "QUIManager.h"

using namespace ui;

QUIManager::QUIManager() {
    main_win = nullptr;
    gl_widget = nullptr;
    ui_quick = nullptr;
    ui_body = nullptr;
    ui_right_hand = nullptr;
    ui_left_hand = nullptr;
    ui_anim = nullptr;
    ui_render_img = nullptr;
    ui_point_cloud = nullptr;
}

QUIManager::~QUIManager() {}

QUIManager &QUIManager::Instance() {
    static QUIManager instance;
    return instance;
}

void QUIManager::Init() {
    this->main_win = new MainWindow();
    main_win->Init();
    main_win->resize(MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT);

    // TO DO
    smodel::ComposedModels &com_models = models_mgr->GetAllModels();
    smodel::ModelCtrlMap &model_ctrls = com_models["SLModel"];
    smodel::ModelCtrlMap::iterator it = model_ctrls.begin();
    while (it != model_ctrls.end()) {
        this->models[it->second->GetName()] = it->second;
        it++;
    }

    this->gl_widget = new GLWidget();
    gl_widget->SetProjectPath(project_path_);
    gl_widget->SetupRenderers(models);
    gl_widget->setMinimumWidth(50);
    gl_widget->setMinimumHeight(50);

    main_win->SetGLWidget(gl_widget);
}

void QUIManager::Show() {
    main_win->show();
}

void QUIManager::UpdateGL() {
    this->gl_widget->update();
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

smodel::ModelCtrl *QUIManager::GetModel(const std::string &name) {
    smodel::models_map::iterator it = models.find(name);
    return it != models.end() ? it->second : nullptr;
}

smodel::models_map &QUIManager::GetModels() {
    return models;
}

QUIQuick *QUIManager::GetUIQuick() {
    return ui_quick;
}

void QUIManager::ShowQuickPannel() {
    if (nullptr == ui_quick) {
        ui_quick = new QUIQuick(project_path_, models, gl_widget->GetRenderers(), gl_widget->GetRenderersState());
        ui_quick->setParent(gl_widget, Qt::WindowType::Dialog);
        ui_quick->Init();
    }
    if (nullptr != ui_quick) {
        bool isVisible = ui_quick->isVisible();
        if (!isVisible) {
            ui_quick->show();
        }
        const QPoint pos = this->main_win->pos();
        ui_quick->move(pos.x() - 250, pos.y());
    }
}

void QUIManager::ShowBodyPannel() {
    smodel::models_map::iterator it = models.find(BODY);
    if (it != models.end()) {
        if (nullptr == ui_body) {
            ui_body = new QUIDashboard(it->second);
            ui_body->setParent(gl_widget, Qt::WindowType::Dialog);
            ui_body->Init();
            ui_body->setWindowTitle("Body Pannel");
        }
        if (nullptr != ui_body) {
            bool isVisible = ui_body->isVisible();
            if (!isVisible) {
                ui_body->show();
            }
            const QPoint pos = this->main_win->pos();
            ui_body->move(pos.x() + GL_WINDOW_WIDTH + 20, pos.y());
        }
    } else {
        std::cout << "no " << BODY << " model" << std::endl;
    }
}

void QUIManager::ShowRightHandPannel() {
    smodel::models_map::iterator it = models.find(RIGHT_HAND);
    if (it != models.end()) {
        if (nullptr == ui_right_hand) {
            ui_right_hand = new QUIDashboard(it->second);
            ui_right_hand->setParent(gl_widget, Qt::WindowType::Dialog);
            ui_right_hand->Init();
            ui_right_hand->setWindowTitle("Right Hand Pannel");
        }
        if (nullptr != ui_right_hand) {
            bool isVisible = ui_right_hand->isVisible();
            if (!isVisible) {
                ui_right_hand->show();
            }
            const QPoint pos = this->main_win->pos();
            ui_right_hand->move(pos.x() + GL_WINDOW_WIDTH + 40, pos.y() + 20);
        }
    } else {
        std::cout << "no " << RIGHT_HAND << " model" << std::endl;
    }
}

void QUIManager::ShowLeftHandPannel() {
    smodel::models_map::iterator it = models.find(LEFT_HAND);
    if (it != models.end()) {
        if (nullptr == ui_left_hand) {
            ui_left_hand = new QUIDashboard(it->second);
            ui_left_hand->setParent(gl_widget, Qt::WindowType::Dialog);
            ui_left_hand->Init();
            ui_left_hand->setWindowTitle("Left Hand Pannel");
        }
        if (nullptr != ui_left_hand) {
            bool isVisible = ui_left_hand->isVisible();
            if (!isVisible) {
                ui_left_hand->show();
            }
            const QPoint pos = this->main_win->pos();
            ui_left_hand->move(pos.x() + GL_WINDOW_WIDTH + 60, pos.y() + 40);
        }
    } else {
        std::cout << "no " << LEFT_HAND << " model" << std::endl;
    }
}

void QUIManager::ShowAnimPannel() {
    if (nullptr == ui_anim) {
        ui_anim = new QUIAnim(project_path_, models, this->signer);
        ui_anim->setParent(gl_widget, Qt::WindowType::Dialog);
        ui_anim->Init();
    }
    if (nullptr != ui_anim) {
        bool isVisible = ui_anim->isVisible();
        if (!isVisible) {
            ui_anim->show();
        }
        const QPoint pos = this->main_win->pos();
        ui_anim->move(pos.x(), pos.y() + ceil(GL_WINDOW_HEIGHT * 4 / 7));
    }
}

void QUIManager::ShowRenderImgPannel() {
    if (nullptr == ui_render_img) {
        ui_render_img = new QUIRenderImg(project_path_, models, gl_widget);
        ui_render_img->setParent(gl_widget, Qt::WindowType::Dialog);
        ui_render_img->Init();
    }
    if (nullptr != ui_render_img) {
        bool isVisible = ui_render_img->isVisible();
        if (!isVisible) {
            ui_render_img->show();
        }
        const QPoint pos = this->main_win->pos();
        ui_render_img->move(pos.x(), pos.y() - 120);
    }
}

void QUIManager::ShowPointCloudWindow(const std::string &file_path) {
    if (nullptr == ui_point_cloud) {
        ui_point_cloud = new QUIPointCloud(project_path_);
    }
    if (nullptr != ui_point_cloud) {
        bool isVisible = ui_point_cloud->isVisible();
        if (!isVisible) {
            ui_point_cloud->SetDepthImgPath(file_path);
            ui_point_cloud->Init();
            if (ui_point_cloud->IsReadSuccess()) {
                ui_point_cloud->show();
            }
            else {
                // QMessageBox::warning(this, tr("Error"), tr("Failed reading depth file!"), QMessageBox::Yes);
                std::cout << "请填写正确的深度图文件" << std::endl;
            }
        }
    }
}

void QUIManager::SetRendererState(const int &id, const bool &is_render) {
    if (nullptr != gl_widget) {
        gl_widget->SetRendererState(id, is_render);
    }
}
