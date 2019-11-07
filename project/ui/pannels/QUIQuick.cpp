#include "QUIQuick.h"
#include "QUIManager.h"

using namespace ui;

QUIQuick::QUIQuick(const std::string &project_path, models_map &models, renderers_map &renderers)
  : project_path_(project_path),
    models_(models),
    renderers_(renderers)
{
    quick_mapper = new QSignalMapper(this);
    checkbox_mapper = new QSignalMapper(this);
    checkbox_btn_mapper = new QSignalMapper(this);
}

QUIQuick::~QUIQuick() {
    init_thetas.clear();
    quick_thetas.clear();
    check_boxes.clear();
    quick_btns.clear();

    delete quick_mapper;
    delete checkbox_mapper;
    delete checkbox_btn_mapper;
}

void QUIQuick::Init() {
    QHBoxLayout *layout = new QHBoxLayout(this); // 总的layout
    layout->setAlignment(Qt::AlignLeft);

    layout->addLayout(this->InitCheckBoxes());
    this->sel_name = models_.begin()->first;
    layout->addLayout(this->InitQuickThetas(sel_name));

    this->ConnectMapper();
    // 总的弹板设置
    this->setLayout(layout);
    this->resize(520, 520);
    this->move(80, 20);
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

    for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
        const std::string &name = it->first;
        // 快捷操作
        QPushButton *btn = new QPushButton();
        btn->setText(QString("%1 >>").arg(name.c_str()));
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setFixedWidth(120);
        btn->setFixedHeight(SINGLE_LINE_HEIGHT);

        connect(btn, SIGNAL(pressed()), checkbox_btn_mapper, SLOT(map()));
        checkbox_btn_mapper->setMapping(btn, QString(name.c_str()));

        layout->addWidget(btn);
    }
    layout->addSpacing(100);
    for (renderers_map::iterator it = renderers_.begin(); it != renderers_.end(); ++it) {
        const int &id = it->first;
        const std::string &name = it->second->GetName();
        // 标签
        QCheckBox *check_box = new QCheckBox(name.c_str());
        check_box->setCheckState(Qt::CheckState::Checked);
        check_box->setFixedWidth(120);
        check_box->setFixedHeight(SINGLE_LINE_HEIGHT);

        connect(check_box, SIGNAL(toggled(bool)), checkbox_mapper, SLOT(map()));
        checkbox_mapper->setMapping(check_box, id);

        check_boxes[id] = check_box;
        layout->addWidget(check_box);
    }

    return layout;
}

QVBoxLayout *QUIQuick::InitQuickThetas(const std::string &name) {
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);

    // 重置按钮
    QPushButton *btn_reset = new QPushButton();
    btn_reset->setText("Reset");
    btn_reset->setFocusPolicy(Qt::NoFocus);
    btn_reset->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);
    btn_reset->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_reset, SIGNAL(pressed()), this, SLOT(onClickBtnReset()));

    layout->addWidget(this->InitQuickScroll(name));
    layout->addWidget(btn_reset);

    return layout;
}

QScrollArea *QUIQuick::InitQuickScroll(const std::string &name) {
    QScrollArea *scroll_area = new QScrollArea();

    this->scroll_content = new QWidget();
    this->scroll_layout = new QGridLayout();

    scroll_layout->setAlignment(Qt::AlignTop);
    scroll_content->setLayout(scroll_layout);
    scroll_content->adjustSize();

    scroll_area->setWidget(scroll_content);
    scroll_area->setWidgetResizable(true);
    scroll_area->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);

    this->UpdateQuickScrollContent(name);

    return scroll_area;
}

void QUIQuick::UpdateQuickScrollContent(const std::string &name) {
    LoadQuickThetas(name);
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
        const std::string &name = quick_thetas[i].first;
        const smodel::Thetas &thetas = quick_thetas[i].second;
        QString qname = QString(name.c_str());

        QPushButton *btn = new QPushButton();
        btn->setText(qname);
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

void QUIQuick::LoadQuickThetas(const std::string &name) {
    quick_thetas.clear();

    models_map::iterator it = models_.find(name);
    if (it != models_.end()) {
        try {
            io::CSVReader<2> centers_csv(project_path_ + "/data/" + name + "/thetas.csv");
            centers_csv.read_header(io::ignore_extra_column, "id", "thetas");
            std::string id, thetas_s;
            int idx = 0;
            const int dofs_size = it->second->GetDofsSize();
            while (centers_csv.read_row(id, thetas_s)) {
                const std::string theta_name = id;
                smodel::Thetas thetas = this->convertToThetas(thetas_s);
                if (thetas.size() != dofs_size) {
                    throw centers_csv.get_file_line();
                }
                quick_thetas.push_back(quick_theta_pair(theta_name, thetas));
            }
        }
        catch (io::error::can_not_open_file err) {
            std::cout << err.what() << std::endl;
        }
        catch (unsigned int line) {
            std::cout << "There's an error at line " << line << "while reading " << name << "'s thetas"<< std::endl;
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

void QUIQuick::UpdateModel(const std::string &name, const smodel::Thetas &thetas) {
    models_[name]->Move(thetas);
    models_[name]->Update();
}

void QUIQuick::UpdateGL() {
    QUIManager::Instance().UpdateGL();
}

void QUIQuick::ApplyModelAs(const std::string &name, const smodel::Thetas &thetas) {
    UpdateModel(name, thetas);
    UpdateGL();
}

void QUIQuick::onCheckBoxToggled(int id) {
    std::map<int, QCheckBox *>::iterator it = check_boxes.find(id);

    if (it != check_boxes.end()) {
        QCheckBox *check_box = check_boxes[id];
        QUIManager::Instance().SetRendererState(id, check_box->checkState() == Qt::CheckState::Checked);
    }
    UpdateGL();
}

void QUIQuick::onModelBtnClick(const QString &name) {
    this->UpdateQuickScrollContent(name.toStdString());
    this->sel_name = name.toStdString();
}

void QUIQuick::onQuickBtnClick(int id) {
    models_map::iterator it = models_.find(this->sel_name);
    if (it != models_.end()) {
        const smodel::Thetas &thetas = quick_thetas[id].second;
        ApplyModelAs(sel_name, thetas);
    }
}

void QUIQuick::onClickBtnReset() {
    models_map::iterator it = models_.find(this->sel_name);
    if (it != models_.end()) {
        it->second->MoveToInit();
        it->second->Update();
        UpdateGL();
    }
}
