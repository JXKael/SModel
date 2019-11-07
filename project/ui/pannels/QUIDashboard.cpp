#include "QUIDashboard.h"
#include "QUIManager.h"

using namespace ui;

QUIDashboard::QUIDashboard(smodel::ModelCtrl *model_ctrl) : model_ctrl_(model_ctrl) {
    this->s2e_mapper = new QSignalMapper(this);
    this->e2s_mapper = new QSignalMapper(this);
}

QUIDashboard::~QUIDashboard() {
    for (auto slider : sliders) {
        delete slider;
    }
    for (auto lineEdit : line_edits) {
        delete lineEdit;
    }
    this->linear_map.clear();
    delete s2e_mapper;
    delete e2s_mapper;
}

void QUIDashboard::Init() {
    if (nullptr == model_ctrl_) {
        return;
    }
    // 自由度设置
    this->InitDofs();

    QHBoxLayout *layout = new QHBoxLayout(this); // 总的layout

    layout->addLayout(this->InitTreePannel());
    layout->addLayout(this->InitThetasPannel());

    this->ConnectMapper();
    // 总的弹板设置
    this->setLayout(layout);
    this->resize(1080, 916);
    this->move(80, 20);
    this->setWindowTitle("Dash board");
}

void QUIDashboard::UpdateModel(const smodel::Thetas &thetas) {
    model_ctrl_->Move(thetas);
    model_ctrl_->Update();
}

void QUIDashboard::UpdateGL() {
    QUIManager::Instance().UpdateGL();
}

void QUIDashboard::ApplyAs(const smodel::Thetas &thetas) {
    s2e_mapper->disconnect();
    e2s_mapper->disconnect();

    for (int i = 0; i < model_ctrl_->GetDofsSize(); ++i) {
        float y = thetas[i];
        float k = linear_map[i][kSlope];
        float b = linear_map[i][kIntercept];
        int x = (int)EDIT_TO_SLIDER_VLAUE(y, k, b);
        ClampTheta(&y, &x, dofs_[i][kMin], dofs_[i][kMax]);
        sliders[i]->setValue(x);
        line_edits[i]->setText(QString("%1").arg(y));
    }
    this->UpdateModel(thetas);
    this->UpdateGL();

    this->ConnectMapper();
}

void QUIDashboard::InitDofs() {
    const smodel::Dofs origin_dofs = model_ctrl_->GetDofs();
    dofs_.resize(origin_dofs.size());
    for (int i = 0; i < origin_dofs.size(); ++i) {
        if (i <= 2) {
            dofs_[i][kMin] = -500;
            dofs_[i][kMax] = 500;
        }
        else {
            dofs_[i][kMin] = origin_dofs[i].min;
            dofs_[i][kMax] = origin_dofs[i].max;
        }
    }
}

QGridLayout *QUIDashboard::InitThetasPannel() {
    sliders.resize(model_ctrl_->GetDofsSize());
    line_edits.resize(model_ctrl_->GetDofsSize());
    linear_map.resize(model_ctrl_->GetDofsSize());
    linear_map.resize(model_ctrl_->GetDofsSize());
    const smodel::Dofs &dofs = model_ctrl_->GetDofs();

    QGridLayout *layout_thetas = new QGridLayout();
    layout_thetas->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    int idx = 0;
    for (float curr_theta : model_ctrl_->GetThetas()) {
        // 计算映射到UI上的参数
        float b = 1;
        float k = 1;
        if (idx >= 0 && idx < this->dofs_.size()) {
            b = dofs_[idx][kMin];
            k = (dofs_[idx][kMax] - dofs_[idx][kMin]) / ADJUST_SLIDER_MAX;
        }
        linear_map[idx][kSlope] = k;
        linear_map[idx][kIntercept] = b;
        // 生成左侧标签
        QLabel *label = new QLabel(QString(model_ctrl_->GetDofs()[idx].name.c_str()));
        label->setFixedWidth(50 * 3);
        label->setFixedHeight(SINGLE_LINE_HEIGHT);
        label->setAlignment(Qt::Alignment::enum_type::AlignRight);
        // 生成中间滑动条
        QSlider *slider = new QSlider(Qt::Horizontal);
        slider->setMinimum(ADJUST_SLIDER_MIN);
        slider->setMaximum(ADJUST_SLIDER_MAX);
        slider->setFixedWidth(50 * 5);
        slider->setFixedHeight(SINGLE_LINE_HEIGHT);
        slider->setPageStep(1);
        slider->setFocusPolicy(Qt::ClickFocus);
        slider->setValue(EDIT_TO_SLIDER_VLAUE(curr_theta, k, b));
        // 生成右侧编辑框
        QLineEdit *lineEdit = new QLineEdit();
        lineEdit->setFixedWidth(50 * 2);
        lineEdit->setFixedHeight(SINGLE_LINE_HEIGHT);
        lineEdit->setText(QString("%1").arg(curr_theta));
        // 绑定滑动消息
        connect(slider, SIGNAL(valueChanged(int)), s2e_mapper, SLOT(map()));
        s2e_mapper->setMapping(slider, idx);
        // 绑定编辑框编辑完成消息（Tab或Enter键触发）
        connect(lineEdit, SIGNAL(editingFinished()), e2s_mapper, SLOT(map()));
        e2s_mapper->setMapping(lineEdit, idx);

        sliders[idx] = slider;
        line_edits[idx] = lineEdit;

        layout_thetas->addWidget(label, idx, 0, 1, 3);
        layout_thetas->addWidget(slider, idx, 3, 1, 5);
        layout_thetas->addWidget(lineEdit, idx, 8, 1, 2);

        if (model_ctrl_->HasParent() && dofs[idx].free_type == smodel::DofFreeType::kUnderCtrl) {
            slider->setDisabled(true);
            lineEdit->setDisabled(true);
        }

        idx++;
    }

    QPushButton *btn_pick = new QPushButton();
    btn_pick->setText("Pick from model");
    btn_pick->setFocusPolicy(Qt::NoFocus);
    btn_pick->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_pick, SIGNAL(pressed()), this, SLOT(onClickBtnPick()));

    layout_thetas->addWidget(btn_pick, idx, 0, 1, 10);

    return layout_thetas;
}

QGridLayout *QUIDashboard::InitTreePannel() {
    QGridLayout *layout_tree = new QGridLayout();

    this->tree = new QTreeWidget(this);
    tree->setHeaderLabels(QStringList() << "Bone name" << "Center id" << "Center name");
    tree->setColumnWidth(0, 240);
    tree->setColumnWidth(1, 80);
    tree->setColumnWidth(2, 160);
    tree->setFixedWidth(500);
    const smodel::Bone &root_bone = model_ctrl_->GetRootBone();
    QTreeWidgetItem *root = new QTreeWidgetItem(QStringList() << root_bone.name.c_str());
    this->AddBoneTreeItem(root_bone, root);
    tree->addTopLevelItem(root);
    connect(tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem *, int)));

    layout_tree->addWidget(this->tree);
    return layout_tree;
}

void QUIDashboard::AddBoneTreeItem(const smodel::Bone &bone, QTreeWidgetItem *parentItem) {
    const smodel::Bones &bones = model_ctrl_->GetBones();
    if (bone.hasChildren()) {
        for (const smodel::Bone *child : bone.children) {
            QString s_centerid = QString("%1").arg(child->center_id);
            QString s_centername = QString("%1").arg(child->center->name.c_str());
            QTreeWidgetItem *item = new QTreeWidgetItem(
                QStringList() << child->name.c_str() <<
                                 s_centerid.toStdString().c_str() <<
                                 s_centername.toStdString().c_str());
            parentItem->addChild(item);
            AddBoneTreeItem(*child, item);
        }
    }
    this->AddAttachCenters(bone, parentItem);
}

void QUIDashboard::AddAttachCenters(const smodel::Bone &bone, QTreeWidgetItem *parentItem) {
    if (bone.hasAttachments()) {
        QTreeWidgetItem *attachment = new QTreeWidgetItem(QStringList() << ATTACHMENT << "the centers attached to this phalange");
        for (int i = 0; i < bone.attachments.size(); ++i) {
            const smodel::Sphere *center = bone.attachments[i];
            QString s_centerid = QString("%1").arg(center->id);
            QString s_centername = QString("%1").arg(center->name.c_str());
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << center->name.c_str() << s_centerid.toStdString().c_str() << s_centername.toStdString().c_str());
            attachment->addChild(item);
        }
        parentItem->addChild(attachment);
    }
}

void QUIDashboard::ConnectMapper() {
    connect(s2e_mapper, SIGNAL(mapped(int)), this, SLOT(sliderHandler(int)));
    connect(e2s_mapper, SIGNAL(mapped(int)), this, SLOT(editHandler(int)));
}

void QUIDashboard::ClampTheta(float *theta, int *slider_val, float min, float max) {
    if (*theta <= min) {
        *slider_val = 0;
        *theta = min;
    }
    if (*theta >= max) {
        *slider_val = ADJUST_SLIDER_MAX;
        *theta = max;
    }
}

void QUIDashboard::onClickBtnPick() {
    ApplyAs(model_ctrl_->GetThetas());
}

void QUIDashboard::sliderHandler(int param) {
    //std::cout << "滑动事件" << endl;
    //std::cout << idx << endl;
    int index = param;
    QSlider *slider = sliders[index];
    QLineEdit *edit = line_edits[index];
    if (nullptr == slider || nullptr == edit) return;

    int x = slider->value();
    float k = linear_map[index][kSlope];
    float b = linear_map[index][kIntercept];
    float y = SLIDER_TO_EDIT_VLAUE(x, k, b);
    // 判断值是否一致，防止循环调用事件
    if (abs(y - edit->text().toFloat()) >= FLT_MIN) {
        edit->setText(QString("%1").arg(y));
        // 更新模型
        smodel::Thetas thetas = model_ctrl_->GetThetas();
        thetas[index] = y;
        this->UpdateModel(thetas);
        this->UpdateGL();
    }
}

void QUIDashboard::editHandler(int param) {
    //std::cout << "编辑事件" << endl;
    //std::cout << idx << endl;
    int index = param;
    QSlider *slider = sliders[index];
    QLineEdit *edit = line_edits[index];
    if (nullptr == slider || nullptr == edit) return;

    float y = edit->text().toFloat();
    float k = linear_map[index][kSlope];
    float b = linear_map[index][kIntercept];
    int x = (int)EDIT_TO_SLIDER_VLAUE(y, k, b);
    ClampTheta(&y, &x, dofs_[index][kMin], dofs_[index][kMax]);

    edit->setText(QString("%1").arg(y));
    // 判断值是否一致，防止循环调用事件
    if (x != slider->value()) {
        slider->setValue(x);
        // 更新模型
        smodel::Thetas thetas = model_ctrl_->GetThetas();
        thetas[index] = y;
        this->UpdateModel(thetas);
        this->UpdateGL();
    }
}

void QUIDashboard::onTreeItemClicked(QTreeWidgetItem *item, int column) {
    int a = 0;
    std::string item_name = item->text(0).toStdString();
    const smodel::Bones &bones = model_ctrl_->GetBones();
    try{
        if (item_name != ATTACHMENT && item_name != ROOT_NAME) {
            int center_id = atoi(item->text(1).toStdString().c_str());
            this->model_ctrl_->SetSelectedCenterid(center_id);
        }
        else throw true;
    }
    catch (bool isError) {
        if (isError) {
            this->model_ctrl_->SetSelectedCenterid();
        }
    }
    QUIManager::Instance().UpdateGL();
}
