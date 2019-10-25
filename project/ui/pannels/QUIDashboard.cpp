#include "QUIDashboard.h"
#include "QUIPannelCtrl.h"

using namespace ui;

QUIDashboard::QUIDashboard(QUIPannelCtrl *pannel_ctrl, smodel::ModelCtrl *model_ctrl) : pannel_ctrl_(pannel_ctrl), model_ctrl_(model_ctrl) {
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

    QHBoxLayout *layout = new QHBoxLayout(); // 总的layout

    layout->addLayout(this->InitThetasPannel());
    layout->addLayout(this->InitTreePannel());

    this->ConnectMapper();
    // 总的弹板设置
    this->setLayout(layout);
    this->resize(1080, 916);
    this->move(80, 20);
    this->setWindowTitle("body pannel");
}

void QUIDashboard::UpdateGL(const smodel::Thetas &thetas) {
    model_ctrl_->Move(thetas);
    model_ctrl_->Update();
    pannel_ctrl_->UpdateGL();
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
    this->UpdateGL(thetas);

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

        idx++;
    }

    return layout_thetas;
}

QGridLayout *QUIDashboard::InitTreePannel() {
    QGridLayout *layout_tree = new QGridLayout();

    this->tree = new QTreeWidget(this);
    tree->setHeaderLabels(QStringList() << "Phalange name" << "Description");
    tree->setColumnWidth(0, 240);
    tree->setColumnWidth(1, 260);
    tree->setFixedWidth(500);
    const smodel::Bones &bones = model_ctrl_->GetBones();
    QTreeWidgetItem *root = new QTreeWidgetItem(QStringList() << bones[0].name.c_str());
    this->AddBoneTreeItem(bones[0], root);
    tree->addTopLevelItem(root);
    connect(tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem *, int)));

    layout_tree->addWidget(this->tree);
    return layout_tree;
}

void QUIDashboard::AddBoneTreeItem(const smodel::Bone &bone, QTreeWidgetItem *parentItem) {
    const smodel::Bones &bones = model_ctrl_->GetBones();
    if (!bone.children_ids.empty()) {
        for (int i = 0; i < bone.children_ids.size(); ++i) {
            const smodel::Bone &child_bone = bones[bone.children_ids[i]];
            int center_id = child_bone.center_id;
            QString s_center = QString("center id: %1, name: %2").arg(center_id).arg(child_bone.center->name.c_str());
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << child_bone.name.c_str() << s_center.toStdString().c_str());
            parentItem->addChild(item);
            AddBoneTreeItem(child_bone, item);
        }
    }
    this->AddAttachCenters(bone, parentItem);
}

void QUIDashboard::AddAttachCenters(const smodel::Bone &bone, QTreeWidgetItem *parentItem) {
    if (bone.attachment_ids.size() > 0) {
        QTreeWidgetItem *attachment = new QTreeWidgetItem(QStringList() << ATTACHMENT << "the centers attached to this phalange");
        for (int i = 0; i < bone.attachments.size(); ++i) {
            const smodel::Sphere *center = bone.attachments[i];
            QString s_center = QString("center id: %1, name: %2").arg(center->id).arg(center->name.c_str());
            attachment->addChild(new QTreeWidgetItem(QStringList() << center->name.c_str() << s_center.toStdString().c_str()));
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
        //switch (tree_item_type) {
        //case kBone:
        //    this->glwidget_->SetSelectBone(selected_boneid);
        //    break;
        //case kAttachment:
        //    this->glwidget_->SetSelectAttachment(selected_boneid, selected_centerid);
        //    break;
        //default:
        //    break;
        //}
        this->UpdateGL(thetas);
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
        //switch (tree_item_type) {
        //case kBone:
        //    this->glwidget_->SetSelectBone(selected_boneid);
        //    break;
        //case kAttachment:
        //    this->glwidget_->SetSelectAttachment(selected_boneid, selected_centerid);
        //    break;
        //default:
        //    break;
        //}
        this->UpdateGL(thetas);
    }
}

void QUIDashboard::onTreeItemClicked(QTreeWidgetItem *item, int column) {
    //int a = 0;
    //std::string itemName = item->text(0).toStdString();
    //const smodel::Bones &bones = model_ctrl_->GetBones();
    //std::map<std::string, int> &bones_name_to_id = model_ctrl_->GetBodyModel()->bones_name_to_id_map;
    //std::map<std::string, int>::iterator it = bones_name_to_id.find(itemName);
    //std::map<std::string, int> &centers_name_to_id = model_ctrl_->GetBodyModel()->centers_name_to_id_map;
    //tree_item_type = kNone;
    //try{
    //    if (itemName != ATTACHMENT) {
    //        if (it != bones_name_to_id.end()){
    //            // 点击的是一个bone
    //            selected_boneid = it->second;
    //            selected_centerid = bones[selected_boneid].center_id;
    //            if (selected_centerid > -1) {
    //                tree_item_type = kBone;
    //                this->glwidget_->SetSelectBone(selected_boneid);
    //            }
    //            else throw true;
    //        }
    //        else {
    //            it = centers_name_to_id.find(itemName);
    //            if (it != centers_name_to_id.end()) {
    //                // 点击的是一个attachment center
    //                selected_centerid = it->second;
    //                std::string phalange_name = item->parent()->parent()->text(0).toStdString();
    //                selected_boneid = bones_name_to_id[phalange_name];
    //                tree_item_type = kAttachment;
    //                this->glwidget_->SetSelectAttachment(selected_boneid, selected_centerid);
    //            }
    //            else throw true;
    //        }
    //    }
    //    else throw true;
    //}
    //catch (bool isError) {
    //    if (isError) {
    //        tree_item_type = kNone;
    //        this->selected_boneid = -1;
    //        this->selected_centerid = -1;
    //        // this->glwidget_->SetSelectPhalange(kRight, -1, false);
    //    }
    //}
    //this->glwidget_->update();
}
