#include "QUIQuick.h"
#include "QUIManager.h"

using namespace ui;

QUIQuick::QUIQuick(const std::string &project_path, models_map &models, renderers_map &renderers, renderers_state_map &renderers_state)
  : project_path_(project_path),
    models_(models),
    renderers_(renderers),
    renderers_state_(renderers_state)
{
    quick_mapper = new QSignalMapper(this);
    checkbox_mapper = new QSignalMapper(this);
    checkbox_btn_mapper = new QSignalMapper(this);

    btn_reset = nullptr;
    btn_save = nullptr;
    edit_pose_name = nullptr;
}

QUIQuick::~QUIQuick() {
    init_thetas.clear();
    quick_thetas.clear();
    check_boxes.clear();
    model_btn.clear();
    quick_btns.clear();
    model_pose.clear();
    pose_mask_map.clear();

    delete quick_mapper;
    delete checkbox_mapper;
    delete checkbox_btn_mapper;

    delete btn_reset;
    delete btn_save;
    delete edit_pose_name;
}

void QUIQuick::Init() {
    QHBoxLayout *layout = new QHBoxLayout(this); // 总的layout
    layout->setAlignment(Qt::AlignLeft);

    this->sel_model_name = models_.begin()->first;
    layout->addLayout(this->InitCheckBoxes());
    layout->addLayout(this->InitQuickThetas(sel_model_name));

    this->ConnectMapper();
    // 总的弹板设置
    this->setLayout(layout);
    this->resize(620, 536);
    this->setWindowTitle("Control Pannel");
}

void QUIQuick::ConnectMapper() {
    connect(quick_mapper, SIGNAL(mapped(int)), this, SLOT(onQuickBtnClick(int)));
    connect(checkbox_mapper, SIGNAL(mapped(int)), this, SLOT(onCheckBoxToggled(int)));
    connect(checkbox_btn_mapper, SIGNAL(mapped(const QString &)), this, SLOT(onModelBtnClick(const QString &)));
}

QVBoxLayout *QUIQuick::InitCheckBoxes() {
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // 模型按钮
    for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
        const std::string &name = it->first;
        QPushButton *btn = new QPushButton();
        QString btn_text = sel_model_name == name ? QString("%1 >>").arg(name.c_str()) : QString("%1").arg(name.c_str());
        btn->setText(btn_text);
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setFixedWidth(180);
        btn->setFixedHeight(SINGLE_LINE_HEIGHT);

        connect(btn, SIGNAL(pressed()), checkbox_btn_mapper, SLOT(map()));
        checkbox_btn_mapper->setMapping(btn, QString(name.c_str()));

        layout->addWidget(btn);

        model_btn[name] = btn;
    }
    layout->addSpacing(100);

    // 渲染器复选框
    for (renderers_map::iterator it = renderers_.begin(); it != renderers_.end(); ++it) {
        const int &id = it->first;
        const std::string &name = it->second->GetName();
        // 标签
        QCheckBox *check_box = new QCheckBox(name.c_str());
        check_box->setCheckState(renderers_state_[id] ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        check_box->setFixedWidth(180);
        check_box->setFixedHeight(SINGLE_LINE_HEIGHT);

        connect(check_box, SIGNAL(toggled(bool)), checkbox_mapper, SLOT(map()));
        checkbox_mapper->setMapping(check_box, id);

        check_boxes[id] = check_box;
        layout->addWidget(check_box);
    }

    return layout;
}

QVBoxLayout *QUIQuick::InitQuickThetas(const std::string &model_name) {
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);

    // 重置按钮
    btn_reset = new QPushButton();
    btn_reset->setText(QString("Reset [%1]'s pose").arg(sel_model_name.c_str()));
    btn_reset->setFocusPolicy(Qt::NoFocus);
    btn_reset->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);
    btn_reset->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_reset, SIGNAL(pressed()), this, SLOT(onClickBtnReset()));

    QHBoxLayout *layout_save = new QHBoxLayout();
    // 保存按钮
    btn_save = new QPushButton();
    btn_save->setText(QString("Save pose to [%1]").arg(sel_model_name.c_str()));
    btn_save->setFocusPolicy(Qt::NoFocus);
    btn_save->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH / 10 * 5);
    btn_save->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_save, SIGNAL(pressed()), this, SLOT(onClickBtnSave()));

    // 保存名称
    edit_pose_name = new QLineEdit();
    edit_pose_name->setFocusPolicy(Qt::ClickFocus);
    edit_pose_name->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH / 10 * 3);
    edit_pose_name->setFixedHeight(SINGLE_LINE_HEIGHT);
    edit_pose_name->setPlaceholderText("pose name");

    // mask值
    edit_mask_val = new QLineEdit();
    edit_mask_val->setFocusPolicy(Qt::ClickFocus);
    edit_mask_val->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH / 10 * 2);
    edit_mask_val->setFixedHeight(SINGLE_LINE_HEIGHT);
    edit_mask_val->setPlaceholderText("mask value");

    layout_save->addWidget(btn_save);
    layout_save->addWidget(edit_pose_name);
    layout_save->addWidget(edit_mask_val);

    // 批渲染RGB按钮
    QPushButton *btn_render_img = new QPushButton();
    btn_render_img->setText("Render screen [Batch]");
    btn_render_img->setFocusPolicy(Qt::NoFocus);
    btn_render_img->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);
    btn_render_img->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_render_img, SIGNAL(pressed()), this, SLOT(onClickBtnRenderImg()));

    layout->addWidget(this->InitQuickScroll(model_name));
    layout->addLayout(layout_save);
    layout->addWidget(btn_reset);
    layout->addWidget(btn_render_img);

    return layout;
}

QScrollArea *QUIQuick::InitQuickScroll(const std::string &model_name) {
    QScrollArea *scroll_area = new QScrollArea();

    this->scroll_content = new QWidget();
    this->scroll_layout = new QGridLayout();

    scroll_layout->setAlignment(Qt::AlignTop);
    scroll_content->setLayout(scroll_layout);
    scroll_content->adjustSize();

    scroll_area->setWidget(scroll_content);
    scroll_area->setWidgetResizable(true);
    scroll_area->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);

    this->UpdateQuickScrollContent(model_name);

    return scroll_area;
}

void QUIQuick::UpdateQuickScrollContent(const std::string &model_name) {
    LoadQuickThetas(model_name);
    for (int i = 0; i < quick_btns.size(); ++i) {
        scroll_layout->removeWidget(quick_btns[i]);
        quick_mapper->removeMappings(quick_btns[i]);
        delete quick_btns[i];
    }
    quick_btns.clear();

    if (quick_thetas.size() <= 0) return;

    if (nullptr == scroll_layout || nullptr == scroll_content) return;

    quick_btns.resize(quick_thetas.size());
    for (int i = 0; i < quick_thetas.size(); ++i) {
        QString pose_name = QString(quick_thetas[i].first.c_str());
        const smodel::Thetas &thetas = quick_thetas[i].second;

        QPushButton *btn = new QPushButton();
        btn->setText(pose_name);
        btn->setFixedHeight(SINGLE_BTN_HEIGHT);
        // btn->setFixedWidth(2 * SINGLE_BTN_HEIGHT);
        btn->setFocusPolicy(Qt::NoFocus);

        // 绑定按钮消息
        connect(btn, SIGNAL(pressed()), quick_mapper, SLOT(map()));
        quick_mapper->setMapping(btn, i);

        scroll_layout->addWidget(btn, i / 4, i % 4);

        quick_btns[i] = btn;
    }
}

void QUIQuick::LoadQuickThetas(const std::string &model_name) {
    quick_thetas.clear();

    models_map::iterator it = models_.find(model_name);
    if (it != models_.end()) {
        try {
            csv::CSVReader<3> centers_csv(project_path_ + "/data/" + model_name + "/thetas.csv");
            centers_csv.read_header(csv::ignore_extra_column, "id", "thetas", "mask");
            std::string id, thetas_s;
            int mask;
            int idx = 0;
            const int dofs_size = it->second->GetDofsSize();
            while (centers_csv.read_row(id, thetas_s, mask)) {
                const std::string theta_name = id;
                smodel::Thetas thetas = this->convertToThetas(thetas_s);
                if (thetas.size() != dofs_size) {
                    throw centers_csv.get_file_line();
                }
                quick_thetas.push_back(quick_theta_pair(theta_name, thetas));
                pose_mask_map[model_name][theta_name] = mask;
            }
        }
        catch (csv::error::can_not_open_file err) {
            std::cout << err.what() << std::endl;
        }
        catch (unsigned int line) {
            std::cout << "There's an error at line " << line << "while reading " << model_name << "'s thetas"<< std::endl;
        }
    }
}

smodel::Thetas QUIQuick::convertToThetas(const std::string &thetas_s) {
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

void QUIQuick::UpdateModel(const std::string &model_name, const smodel::Thetas &thetas) {
    models_[model_name]->Move(thetas);
    models_[model_name]->Update();
}

void QUIQuick::UpdateGL() {
    QUIManager::Instance().UpdateGL();
}

void QUIQuick::ApplyModelAs(const std::string &model_name, const smodel::Thetas &thetas) {
    UpdateModel(model_name, thetas);
    UpdateGL();
}

std::map<std::string, int> QUIQuick::GetModelMaskVals() {
    std::map<std::string, int> mask_vals;
    for (std::map<std::string, std::string>::iterator it = model_pose.begin(); it != model_pose.end(); ++it) {
        mask_vals[it->first] = pose_mask_map[it->first][it->second];
    }
    return mask_vals;
}

quick_thetas_list &QUIQuick::GetQuickThetas() {
    return this->quick_thetas;
}

void QUIQuick::onCheckBoxToggled(int id) {
    std::map<int, QCheckBox *>::iterator it = check_boxes.find(id);

    if (it != check_boxes.end()) {
        QCheckBox *check_box = it->second;
        bool is_show = check_box->checkState() == Qt::CheckState::Checked;
        QUIManager::Instance().SetRendererState(id, is_show);
        renderers_state_[id] = is_show;
    }
    UpdateGL();
}

void QUIQuick::onModelBtnClick(const QString &name) {
    this->sel_model_name = name.toStdString();
    for (std::map<std::string, QPushButton *>::iterator it = model_btn.begin(); it != model_btn.end(); ++it) {
        QString btn_text = sel_model_name == it->first ? QString("%1 >>").arg(it->first.c_str()) : QString("%1").arg(it->first.c_str());
        it->second->setText(btn_text);
        btn_reset->setText(QString("Reset [%1]'s pose").arg(sel_model_name.c_str()));
        btn_save->setText(QString("Save pose to [%1]").arg(sel_model_name.c_str()));
    }
    this->UpdateQuickScrollContent(name.toStdString());
}

void QUIQuick::onQuickBtnClick(int id) {
    models_map::iterator it = models_.find(this->sel_model_name);
    if (it != models_.end()) {
        model_pose[sel_model_name] = quick_thetas[id].first;
        const smodel::Thetas &thetas = quick_thetas[id].second;
        models_[sel_model_name]->SetMaskValue(pose_mask_map[sel_model_name][quick_thetas[id].first]);
        ApplyModelAs(sel_model_name, thetas);
    }
}

void QUIQuick::onClickBtnSave() {
    const std::string pose_name = edit_pose_name->text().toStdString();
    const std::string mask_val = edit_mask_val->text().toStdString();
    if (pose_name.size() > 0 && mask_val.size() > 0) {
        int mask = atoi(mask_val.c_str());
        if (mask <= 0 || mask >= 255) {
            QMessageBox::warning(this, tr("Error"), tr("mask value should upper than 0\nand lower than 255"), QMessageBox::Yes);
            std::cout << "mask值必须大于0小于255" << std::endl;
            return;
        }
        const std::string file_path = project_path_ + "/data/" + sel_model_name + "/thetas.csv";
        std::ofstream fout;
        try {
            fout.open(file_path, std::ios::app | std::ios::out);
            if (fout.is_open()) {
                // 名称
                fout << pose_name << ",";
                // 参数
                const smodel::Thetas &thetas = models_[sel_model_name]->GetThetas();
                for (int i = 0; i < thetas.size(); ++i) {
                    i < thetas.size() - 1 ? fout << thetas[i] << "#" : fout << thetas[i] << ",";
                }
                // mask值
                fout << mask << std::endl;

                // 清理
                fout.clear();
                fout.close();
                std::cout << "保存姿势[" << pose_name << "]成功" << std::endl;
                edit_pose_name->clear();
                edit_pose_name->clearFocus();
                edit_mask_val->clear();
                edit_mask_val->clearFocus();
                // 重新读取
                this->onModelBtnClick(QString("%1").arg(sel_model_name.c_str()));
                QMessageBox::information(this, tr("Success"), tr("Save Success!"), QMessageBox::Yes);
            }
            else {
                QMessageBox::warning(this, tr("Error"), tr("File Opened Error"), QMessageBox::Yes);
                std::cout << "文件打开失败" << std::endl;
                return;
            }
        }
        catch (std::ios::failure) {
            QMessageBox::warning(this, tr("Error"), tr("Opening File Error"), QMessageBox::Yes);
            std::cout << "打开文件失败" << std::endl;
            return;
        }
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the pose name and mask value"), QMessageBox::Yes);
        std::cout << "请填写姿势名称和遮罩值" << std::endl;
        return;
    }
}

void QUIQuick::onClickBtnReset() {
    models_map::iterator it = models_.find(this->sel_model_name);
    if (it != models_.end()) {
        it->second->MoveToInit();
        it->second->Update();
        UpdateGL();
    }
}

void QUIQuick::onClickBtnRenderImg() {
    QUIManager::Instance().ShowRenderImgPannel();
}
