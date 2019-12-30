#include "QUIAnim.h"
#include "QUIManager.h"

using namespace ui;

QUIAnim::QUIAnim(const std::string &project_path, std::map<std::string, smodel::ModelCtrl *> &models)
  : project_path_(project_path),
    models_(models)
{
    max_frame = 120;
    curr_frame = 0;
    timer = new QTimer(this);

    sliders_mapper = new QSignalMapper(this);
    keys_mapper = new QSignalMapper(this);
}

QUIAnim::~QUIAnim() {
    delete timer;
    delete sliders_mapper;
    delete keys_mapper;
}

void QUIAnim::Init() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    layout->addLayout(InitHead());
    for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
        std::string name = it->first;
        smodel::ModelCtrl *model = it->second;
        layout->addLayout(InitTimeline(name, model));
    }
    layout->addLayout(InitOperation());

    connect(sliders_mapper, SIGNAL(mapped(const QString &)), this, SLOT(onSliderValueChanged(const QString &)));
    connect(keys_mapper, SIGNAL(mapped(const QString &)), this, SLOT(onClickKey(const QString &)));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));

    // 总的弹板设置
    this->setLayout(layout);
    this->resize(1400, 400);
    this->move(80, 20);
    this->setWindowTitle("Animation");
}

QHBoxLayout *QUIAnim::InitHead() {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft);

    // 最大帧数
    QLabel *label_maxframe = new QLabel("max frame");
    label_maxframe->setFixedWidth(SINGLE_BTN_WIDTH);
    label_maxframe->setFixedHeight(SINGLE_LINE_HEIGHT);
    label_maxframe->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    this->edit_maxframe = new QLineEdit();
    edit_maxframe->setFixedWidth(SINGLE_BTN_WIDTH);
    edit_maxframe->setFixedHeight(SINGLE_LINE_HEIGHT);
    edit_maxframe->setText(QString("%1").arg(this->max_frame));

    // 当前帧数
    QLabel *label_currframe = new QLabel("current frame");
    label_currframe->setFixedWidth(SINGLE_BTN_WIDTH);
    label_currframe->setFixedHeight(SINGLE_LINE_HEIGHT);
    label_currframe->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    this->edit_currframe = new QLineEdit(QString("%1").arg(this->curr_frame));
    edit_currframe->setFixedWidth(SINGLE_BTN_WIDTH);
    edit_currframe->setFixedHeight(SINGLE_LINE_HEIGHT);
    edit_currframe->setFixedWidth(SINGLE_BTN_WIDTH);

    // fps
    QLabel *label_fps = new QLabel("fps");
    label_currframe->setFixedWidth(SINGLE_BTN_WIDTH);
    label_currframe->setFixedHeight(SINGLE_LINE_HEIGHT);
    label_currframe->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    this->edit_fps = new QLineEdit(QString("%1").arg(this->curr_frame));
    edit_fps->setFixedWidth(SINGLE_BTN_WIDTH);
    edit_fps->setFixedHeight(SINGLE_LINE_HEIGHT);
    edit_fps->setFixedWidth(SINGLE_BTN_WIDTH);
    edit_fps->setText(QString("%1").arg(this->fps));

    // 播放/暂停按钮
    this->btn_play = new QPushButton();
    btn_play->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_play->setFixedHeight(SINGLE_LINE_HEIGHT);
    btn_play->setText(btnPlayText());
    connect(btn_play, SIGNAL(pressed()), this, SLOT(onClickPlay()));

    // 停止播放按钮
    this->btn_stop = new QPushButton();
    btn_stop->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_stop->setFixedHeight(SINGLE_LINE_HEIGHT);
    btn_stop->setText("stop");
    connect(btn_stop, SIGNAL(pressed()), this, SLOT(onClickStop()));

    layout->addWidget(label_maxframe);
    layout->addWidget(edit_maxframe);
    layout->addSpacing(80);
    layout->addWidget(label_currframe);
    layout->addWidget(edit_currframe);
    layout->addSpacing(80);
    layout->addWidget(label_fps);
    layout->addWidget(edit_fps);
    layout->addStretch();
    layout->addWidget(btn_play);
    layout->addWidget(btn_stop);

    return layout;
}

QHBoxLayout *QUIAnim::InitTimeline(const std::string &name, const smodel::ModelCtrl *model) {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft);

    QLabel *label_righthand = new QLabel(name.c_str());
    label_righthand->setFixedWidth(SINGLE_BTN_WIDTH);
    label_righthand->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 时间轴
    QSlider *timeline = new QSlider(Qt::Horizontal);
    timeline->setMinimum(0);
    timeline->setMaximum(this->max_frame);
    timeline->setPageStep(1);
    timeline->setValue(0);
    timeline->setFixedWidth(timeline_width);
    // 绑定滑动消息
    connect(timeline, SIGNAL(valueChanged(int)), sliders_mapper, SLOT(map()));
    sliders_mapper->setMapping(timeline, QString(name.c_str()));
    sliders[name] = timeline;
    // K帧按钮
    QPushButton *btn_key = new QPushButton();
    btn_key->setText("key");
    btn_key->setFixedWidth(SINGLE_BTN_WIDTH);
    connect(btn_key, SIGNAL(pressed()), keys_mapper, SLOT(map()));
    keys_mapper->setMapping(btn_key, QString(name.c_str()));

    layout->addWidget(label_righthand);
    layout->addWidget(timeline);
    layout->addStretch();
    layout->addWidget(btn_key);

    return layout;
}

QHBoxLayout *QUIAnim::InitOperation() {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignLeft);
    // 文件路径
    file_path = new QLineEdit();
    file_path->setFocusPolicy(Qt::ClickFocus);
    file_path->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);
    file_path->setFixedHeight(SINGLE_LINE_HEIGHT);
    file_path->setText(QString("%1/data/signs/sign_data/").arg(project_path_.c_str()));

    // 文件名称
    file_name = new QLineEdit();
    file_name->setFocusPolicy(Qt::ClickFocus);
    file_name->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH / 4);
    file_name->setFixedHeight(SINGLE_LINE_HEIGHT);
    file_name->setPlaceholderText("sign id");

    // 保存按钮
    QPushButton *btn_save = new QPushButton();
    btn_save->setText("Save");
    btn_save->setFocusPolicy(Qt::ClickFocus);
    btn_save->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_save->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_save, SIGNAL(pressed()), this, SLOT(onClickBtnSave()));

    layout->addWidget(file_path);
    layout->addWidget(file_name);
    layout->addWidget(btn_save);

    return layout;
}

void QUIAnim::UpdateModel(const std::string &name, const smodel::Thetas &thetas) {
    models_[name]->Move(thetas);
    models_[name]->Update();
}

void QUIAnim::UpdateGL() {
    QUIManager::Instance().UpdateGL();
}

const smodel::Thetas QUIAnim::calcThetas(const std::string &name) {
    std::vector<int> keyframes;
    smodel::ModelCtrl *model = models_[name];
    std::map<int, smodel::Thetas> &key_points = keypoints[name];
    for (const std::pair<int, smodel::Thetas> &keypoint : key_points) {
        keyframes.push_back(keypoint.first);
    }
    for (int i = 0; i < keyframes.size(); ++i) {
        if (i + 1 < keyframes.size()) {
            if (curr_frame < keyframes[0]) {
                return key_points[keyframes[0]];
            }
            else if (curr_frame >= keyframes[keyframes.size() - 1]) {
                return key_points[keyframes[keyframes.size() - 1]];
            }
            else if (curr_frame >= keyframes[i] && curr_frame < keyframes[i + 1]) {
                std::vector<float> thetas;
                int startFrame = keyframes[i];
                int endFrame = keyframes[i + 1];
                for (int j = 0; j < model->GetDofsSize(); ++j) {
                    float startTheta = key_points[startFrame][j];
                    float endTheta = key_points[endFrame][j];
                    float k = (endTheta - startTheta) / (endFrame - startFrame);
                    float currTheta = k * (curr_frame - startFrame) + startTheta;
                    thetas.push_back(currTheta);
                }
                return thetas;
            }
        }
        else {
            return key_points[keyframes[i]];
        }
    }
    return model->GetThetas();
}

void QUIAnim::onClickPlay() {
    if (!this->isPlaying()) {
        if (curr_frame >= max_frame) curr_frame = 0;
        int interval = (int)(1000.0f / (float)this->fps);
        this->timer->start(interval);
    } else {
        this->timer->stop();
    }
    this->btn_play->setText(btnPlayText());
}

void QUIAnim::onClickStop() {
    this->timer->stop();
    this->curr_frame = 0;
    std::map<std::string, QSlider *>::iterator it = sliders.begin();
    it->second->setValue(curr_frame);
    this->btn_play->setText(btnPlayText());
}

void QUIAnim::onSliderValueChanged(const QString &name) {
    // std::cout << name.toStdString() << std::endl;

    std::string model_name = name.toStdString();
    curr_frame = this->sliders[model_name]->value();
    edit_currframe->setText(QString("%1").arg(curr_frame));
    for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
        this->UpdateModel(it->first, calcThetas(it->first));
    }

    this->UpdateGL();

    disconnect(sliders_mapper, SIGNAL(mapped(const QString &)), this, SLOT(onSliderValueChanged(const QString &)));
    for (const std::pair<std::string, QSlider *> slider : sliders) {
        slider.second->setValue(curr_frame);
    }
    connect(sliders_mapper, SIGNAL(mapped(const QString &)), this, SLOT(onSliderValueChanged(const QString &)));
}

void QUIAnim::onClickKey(const QString &name) {
    // std::cout << name.toStdString() << std::endl;

    std::string model_name = name.toStdString();
    this->keypoints[model_name][this->curr_frame] = this->models_[model_name]->GetThetas();
}

void QUIAnim::updateTimer() {
    if (this->curr_frame <= this->max_frame) {
        std::map<std::string, QSlider *>::iterator it = sliders.begin();
        it->second->setValue(curr_frame);
        ++curr_frame;
    } else {
        this->onClickPlay();
    }
}

void QUIAnim::onClickBtnSave() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        const std::string file = (file_path->text() + file_name->text()).toStdString();
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the path & id"), QMessageBox::Yes);
    }
}
