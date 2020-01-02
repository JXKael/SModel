#include "QUIRenderImg.h"
#include "QUIManager.h"

using namespace ui;

smodel::Thetas convertToThetas(const std::string &thetas_s);

QUIRenderImg::QUIRenderImg(const std::string &project_path, models_map &models, GLWindow *gl_window)
  : project_path_(project_path),
    models_(models),
    gl_window_(gl_window)
{

}

QUIRenderImg::~QUIRenderImg() {}

void QUIRenderImg::Init() {
    QGridLayout *layout = new QGridLayout(this);
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // 文件路径
    file_path = new QLineEdit();
    file_path->setFocusPolicy(Qt::ClickFocus);
    file_path->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);
    file_path->setFixedHeight(SINGLE_LINE_HEIGHT);
    file_path->setText(QString("%1/data/images/").arg(project_path_.c_str()));

    // 文件名称
    file_name = new QLineEdit();
    file_name->setFocusPolicy(Qt::ClickFocus);
    file_name->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH / 4);
    file_name->setFixedHeight(SINGLE_LINE_HEIGHT);
    file_name->setPlaceholderText("file name");

    // 文件后缀
    file_suffix = new QLabel();
    file_suffix->setFocusPolicy(Qt::NoFocus);
    file_suffix->setFixedHeight(SINGLE_LINE_HEIGHT);
    file_suffix->setText(".png");

    // 设置屏幕为渲染手部
    QPushButton *btn_norm = new QPushButton();
    btn_norm->setText("Normlize Windown and View");
    btn_norm->setFocusPolicy(Qt::ClickFocus);
    btn_norm->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_norm, SIGNAL(pressed()), this, SLOT(onClickBtnNormScreen()));
    // 重置屏幕
    QPushButton *btn_norm_reset = new QPushButton();
    btn_norm_reset->setText("Reset");
    btn_norm_reset->setFocusPolicy(Qt::ClickFocus);
    btn_norm_reset->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_norm_reset, SIGNAL(pressed()), this, SLOT(onClickBtnResetScreen()));

    // 单张渲染
    QPushButton *btn_render_one = new QPushButton();
    btn_render_one->setText("Render [RGB] One");
    btn_render_one->setFocusPolicy(Qt::ClickFocus);
    btn_render_one->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_one, SIGNAL(pressed()), this, SLOT(onClickBtnRenderOne()));
    // 批量渲染
    QPushButton *btn_render_batch = new QPushButton();
    btn_render_batch->setText("Render [RGB] Batch");
    btn_render_batch->setFocusPolicy(Qt::ClickFocus);
    btn_render_batch->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_batch, SIGNAL(pressed()), this, SLOT(onClickBtnRenderBatch()));

    // mask 单张
    QPushButton *btn_render_mask_one = new QPushButton();
    btn_render_mask_one->setText("Render [Mask] One");
    btn_render_mask_one->setFocusPolicy(Qt::ClickFocus);
    btn_render_mask_one->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_mask_one, SIGNAL(pressed()), this, SLOT(onClickBtnRenderMaskOne()));
    // mask 批量
    QPushButton *btn_render_mask_batch = new QPushButton();
    btn_render_mask_batch->setText("Render [Mask] Batch");
    btn_render_mask_batch->setFocusPolicy(Qt::ClickFocus);
    btn_render_mask_batch->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_mask_batch, SIGNAL(pressed()), this, SLOT(onClickBtnRenderMaskBatch()));

    // finger-mask 单张
    QPushButton *btn_render_finger_mask_one = new QPushButton();
    btn_render_finger_mask_one->setText("Render [Finger Mask] One");
    btn_render_finger_mask_one->setFocusPolicy(Qt::ClickFocus);
    btn_render_finger_mask_one->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_finger_mask_one, SIGNAL(pressed()), this, SLOT(onClickBtnRenderFingerMaskOne()));
    // finger-mask 批量
    QPushButton *btn_render_finger_mask_batch = new QPushButton();
    btn_render_finger_mask_batch->setText("Render [Finger Mask] Batch");
    btn_render_finger_mask_batch->setFocusPolicy(Qt::ClickFocus);
    btn_render_finger_mask_batch->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_finger_mask_batch, SIGNAL(pressed()), this, SLOT(onClickBtnRenderFingerMaskBatch()));

    // one for all
    QPushButton *btn_one_for_all = new QPushButton();
    btn_one_for_all->setText("One For All [RGB]");
    btn_one_for_all->setFocusPolicy(Qt::ClickFocus);
    btn_one_for_all->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_one_for_all, SIGNAL(pressed()), this, SLOT(onClickBtnOneForAll()));

    QPushButton *btn_one_for_all_mask = new QPushButton();
    btn_one_for_all_mask->setText("One For All [Mask]");
    btn_one_for_all_mask->setFocusPolicy(Qt::ClickFocus);
    btn_one_for_all_mask->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_one_for_all_mask, SIGNAL(pressed()), this, SLOT(onClickBtnOneForAllMask()));

    QPushButton *btn_one_for_all_finger_mask = new QPushButton();
    btn_one_for_all_finger_mask->setText("One For All [Finger Mask]");
    btn_one_for_all_finger_mask->setFocusPolicy(Qt::ClickFocus);
    btn_one_for_all_finger_mask->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_one_for_all_finger_mask, SIGNAL(pressed()), this, SLOT(onClickBtnOneForAllFingerMask()));


    layout->addWidget(file_path, 0, 0, 1, 4);
    layout->addWidget(file_name, 0, 4, 1, 2);
    layout->addWidget(file_suffix, 0, 7, 1, 1);

    layout->addWidget(btn_norm, 1, 0, 1, 3);
    layout->addWidget(btn_norm_reset, 1, 3, 1, 1);

    layout->addWidget(btn_render_one, 2, 0, 1, 2);
    layout->addWidget(btn_render_batch, 2, 2, 1, 2);

    layout->addWidget(btn_render_mask_one, 3, 0, 1, 2);
    layout->addWidget(btn_render_mask_batch, 3, 2, 1, 2);

    layout->addWidget(btn_render_finger_mask_one, 4, 0, 1, 2);
    layout->addWidget(btn_render_finger_mask_batch, 4, 2, 1, 2);

    layout->addWidget(btn_one_for_all, 5, 0, 1, 2);
    layout->addWidget(btn_one_for_all_mask, 5, 2, 1, 2);
    layout->addWidget(btn_one_for_all_finger_mask, 6, 0, 1, 2);

    // 总的弹板设置
    this->setLayout(layout);
    this->resize(600, 100);
    this->setWindowTitle("Render Image");
}

void QUIRenderImg::UpdateModel(const std::string &name, const smodel::Thetas &thetas) {
    models_[name]->Move(thetas);
    models_[name]->Update();
}

void QUIRenderImg::UpdateGL() {
    QUIManager::Instance().UpdateGL();
}

void QUIRenderImg::onClickBtnNormScreen() {
    gl_window_->SetRenderHandScreen();
}

void QUIRenderImg::onClickBtnResetScreen() {
    gl_window_->ResetScreen();
}


void QUIRenderImg::onClickBtnRenderOne() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        const std::string file = (file_path->text() + file_name->text() + ".png").toStdString();
        gl_window_->SaveScreenImg(file);
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the image's path & name"), QMessageBox::Yes);
    }
}

void QUIRenderImg::onClickBtnRenderBatch() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        gl_window_->SaveScreenImgBatch(file_path->text().toStdString(), file_name->text().toStdString(), "RGB", "png");
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the image's path & name"), QMessageBox::Yes);
    }
}


void QUIRenderImg::onClickBtnRenderMaskOne() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        const std::string file = (file_path->text() + file_name->text() + "-mask.png").toStdString();
        gl_window_->SaveScreenImgMask(file);
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the image's path & name"), QMessageBox::Yes);
    }
}

void QUIRenderImg::onClickBtnRenderMaskBatch() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        gl_window_->SaveScreenImgBatch(file_path->text().toStdString(), file_name->text().toStdString(), "mask", "png", true);
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the image's path & name"), QMessageBox::Yes);
    }
}

void QUIRenderImg::onClickBtnRenderFingerMaskOne() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        const std::string file = (file_path->text() + file_name->text() + "-finger-mask.png").toStdString();
        gl_window_->SaveScreenImgMask(file);
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the image's path & name"), QMessageBox::Yes);
    }
}

void QUIRenderImg::onClickBtnRenderFingerMaskBatch() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        gl_window_->SaveScreenImgBatch(file_path->text().toStdString(), file_name->text().toStdString(), "finger-mask", "png", true);
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the image's path & name"), QMessageBox::Yes);
    }
}



void QUIRenderImg::onClickBtnOneForAll() {
    std::vector<std::string> all_pose = { "A", "B", "C", "D", "E", "F", "G",
                                          "H", "I", "J", "K", "L", "M", "N",
                                          "O", "P", "Q",      "R", "S", "T",
                                          "U", "V", "W",      "X", "Y", "Z",
                                          "ZH", "CH", "SH", "NG" };
    QUIQuick *ui_quick = QUIManager::Instance().GetUIQuick();
    if (nullptr != ui_quick) {
        for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
            const std::string &model_name = it->first;
            smodel::ModelCtrl *model_ctrl = it->second;
            loadQuickThetas(project_path_, model_name, model_ctrl);
            if (pose_thetas_map[model_name].size() < all_pose.size()) {
                std::cout << "模型" << model_name << "快捷参数有问题" << std::endl;
                return;
            }
        }
        for (const std::string &pose_name : all_pose) {
            std::cout << "--> About to render [" << std::setw(2) << pose_name << "] ----------------------------------------------" << std::endl;
            bool failure = true;
            for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
                const std::string &model_name = it->first;
                quick_thetas_list &quick_thetas = pose_thetas_map[model_name];
                for (const quick_theta_pair &theta_pair : quick_thetas) {
                    if (theta_pair.first == pose_name) {
                        std::cout << "--> [" << model_name << "]'s pose: " << pose_name << " was found" << std::endl;
                        this->UpdateModel(model_name, theta_pair.second);
                        failure = false;
                        break;
                    }
                }
                if (failure) {
                    std::cout << "!!! Warning " << "[" << model_name << "]'s pose: " << pose_name << " was not found" << std::endl;
                }
            }
            if (!failure) {
                file_name->setText(pose_name.c_str());
                this->UpdateGL();
                this->onClickBtnRenderBatch();
            }
            else {
                std::cout << "--> Render process passed!!!" << std::endl;
            }
        }
    }
}

void QUIRenderImg::onClickBtnOneForAllMask() {
    std::vector<std::string> all_pose = { "A", "B", "C", "D", "E", "F", "G",
                                          "H", "I", "J", "K", "L", "M", "N",
                                          "O", "P", "Q",      "R", "S", "T",
                                          "U", "V", "W",      "X", "Y", "Z",
                                          "ZH", "CH", "SH", "NG" };
    QUIQuick *ui_quick = QUIManager::Instance().GetUIQuick();
    if (nullptr != ui_quick) {
        for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
            const std::string &model_name = it->first;
            smodel::ModelCtrl *model_ctrl = it->second;
            loadQuickThetas(project_path_, model_name, model_ctrl);
            if (pose_thetas_map[model_name].size() < all_pose.size()) {
                std::cout << "模型" << model_name << "快捷参数有问题" << std::endl;
                return;
            }
            if (pose_mask_map[model_name].size() < all_pose.size()) {
                std::cout << "模型" << model_name << "mask有问题" << std::endl;
                return;
            }
        }
        for (const std::string &pose_name : all_pose) {
            std::cout << "--> About to render [" << std::setw(2) << pose_name << "] ----------------------------------------------" << std::endl;
            bool failure = true;
            for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
                const std::string &model_name = it->first;
                quick_thetas_list &quick_thetas = pose_thetas_map[model_name];
                quick_mask_list &quick_mask = pose_mask_map[model_name];
                for (int i = 0; i < quick_thetas.size(); ++i) {
                    const quick_theta_pair &theta_pair = quick_thetas[i];
                    const quick_mask_pair &mask_pair = quick_mask[i];
                    if (theta_pair.first == pose_name && mask_pair.first == pose_name) {
                        std::cout << "--> [" << model_name << "]'s pose: " << pose_name << " was found" << std::endl;
                        this->UpdateModel(model_name, theta_pair.second);
                        // special
                        models_[model_name]->SetMaskValue(mask_pair.second);
                        failure = false;
                        break;
                    }
                }
                if (failure) {
                    std::cout << "!!! Warning " << "[" << model_name << "]'s pose: " << pose_name << " was not found" << std::endl;
                }
            }
            if (!failure) {
                file_name->setText(pose_name.c_str());
                this->UpdateGL();
                this->onClickBtnRenderMaskBatch();
            }
            else {
                std::cout << "--> Render process passed!!!" << std::endl;
            }
        }
    }
}

void QUIRenderImg::onClickBtnOneForAllFingerMask() {
    std::vector<std::string> all_pose = { "A", "B", "C", "D", "E", "F", "G",
                                          "H", "I", "J", "K", "L", "M", "N",
                                          "O", "P", "Q",      "R", "S", "T",
                                          "U", "V", "W",      "X", "Y", "Z",
                                          "ZH", "CH", "SH", "NG" };
    QUIQuick *ui_quick = QUIManager::Instance().GetUIQuick();
    if (nullptr != ui_quick) {
        for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
            const std::string &model_name = it->first;
            smodel::ModelCtrl *model_ctrl = it->second;
            loadQuickThetas(project_path_, model_name, model_ctrl);
            if (pose_thetas_map[model_name].size() < all_pose.size()) {
                std::cout << "模型" << model_name << "快捷参数有问题" << std::endl;
                return;
            }
            if (pose_mask_map[model_name].size() < all_pose.size()) {
                std::cout << "模型" << model_name << "mask有问题" << std::endl;
                return;
            }
        }
        for (const std::string &pose_name : all_pose) {
            std::cout << "--> About to render [" << std::setw(2) << pose_name << "] ----------------------------------------------" << std::endl;
            bool failure = true;
            for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
                const std::string &model_name = it->first;
                quick_thetas_list &quick_thetas = pose_thetas_map[model_name];
                quick_mask_list &quick_mask = pose_mask_map[model_name];
                for (int i = 0; i < quick_thetas.size(); ++i) {
                    const quick_theta_pair &theta_pair = quick_thetas[i];
                    const quick_mask_pair &mask_pair = quick_mask[i];
                    if (theta_pair.first == pose_name && mask_pair.first == pose_name) {
                        std::cout << "--> [" << model_name << "]'s pose: " << pose_name << " was found" << std::endl;
                        this->UpdateModel(model_name, theta_pair.second);
                        failure = false;
                        break;
                    }
                }
                if (failure) {
                    std::cout << "!!! Warning " << "[" << model_name << "]'s pose: " << pose_name << " was not found" << std::endl;
                }
            }
            if (!failure) {
                file_name->setText(pose_name.c_str());
                this->UpdateGL();
                this->onClickBtnRenderFingerMaskBatch();
            }
            else {
                std::cout << "--> Render process passed!!!" << std::endl;
            }
        }
    }
}

void QUIRenderImg::loadQuickThetas(const std::string &project_path, const std::string &model_name, smodel::ModelCtrl *model_ctrl) {
    quick_thetas_list quick_thetas;

    if (nullptr != model_ctrl) {
        try {
            csv::CSVReader<3> centers_csv(project_path + "/data/" + model_name + "/thetas.csv");
            centers_csv.read_header(csv::ignore_extra_column, "id", "thetas", "mask");
            std::string id, thetas_s;
            int mask;
            int idx = 0;
            const int dofs_size = model_ctrl->GetDofsSize();
            while (centers_csv.read_row(id, thetas_s, mask)) {
                const std::string theta_name = id;
                smodel::Thetas thetas = convertToThetas(thetas_s);
                if (thetas.size() != dofs_size) {
                    throw centers_csv.get_file_line();
                }
                pose_thetas_map[model_name].push_back(quick_theta_pair(theta_name, thetas));
                pose_mask_map[model_name].push_back(quick_mask_pair(theta_name, mask));
            }
        }
        catch (csv::error::can_not_open_file err) {
            std::cout << err.what() << std::endl;
        }
        catch (unsigned int line) {
            std::cout << "There's an error at line " << line << "while reading " << model_name << "'s thetas" << std::endl;
        }
    }
}

smodel::Thetas convertToThetas(const std::string &thetas_s) {
    smodel::Thetas res;
    if (thetas_s.size() > 0) {
        std::stringstream ss(thetas_s);
        std::string val;
        while (getline(ss, val, '#')) {
            res.push_back((float)atof(val.c_str()));
        }
    }
    return res;
}
