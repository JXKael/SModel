#include "QUIAnim.h"
#include "QUIManager.h"

#include "../../signer/SignData.h"

using namespace ui;

QUIAnim::QUIAnim(const std::string &project_path, models_map &models, signs::Signer *signer)
  : project_path_(project_path),
    models_(models),
    signer_(signer)
{
    max_frame = 120;
    curr_frame = 0;
    timer = new QTimer(this);

    sliders_mapper = new QSignalMapper(this);
    keys_mapper = new QSignalMapper(this);
    signs_mapper = new QSignalMapper(this);
}

QUIAnim::~QUIAnim() {
    delete timer;
    delete sliders_mapper;
    delete keys_mapper;
    delete signs_mapper;
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
    layout->addWidget(InitSignsScroll());

    connect(sliders_mapper, SIGNAL(mapped(const QString &)), this, SLOT(onSliderValueChanged(const QString &)));
    connect(keys_mapper, SIGNAL(mapped(const QString &)), this, SLOT(onClickKey(const QString &)));
    connect(signs_mapper, SIGNAL(mapped(const int &)), this, SLOT(onClickSignsBtn(const int &)));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));

    // 总的弹板设置
    this->setLayout(layout);
    this->resize(GL_WINDOW_WIDTH, 400);
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
    connect(edit_maxframe, SIGNAL(editingFinished()), this, SLOT(onMaxFrameEditingFinished()));

    // 当前帧数
    QLabel *label_currframe = new QLabel("current frame");
    label_currframe->setFixedWidth(SINGLE_BTN_WIDTH);
    label_currframe->setFixedHeight(SINGLE_LINE_HEIGHT);
    label_currframe->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    this->edit_currframe = new QLineEdit(QString("%1").arg(this->curr_frame));
    edit_currframe->setFixedWidth(SINGLE_BTN_WIDTH);
    edit_currframe->setFixedHeight(SINGLE_LINE_HEIGHT);
    edit_currframe->setFixedWidth(SINGLE_BTN_WIDTH);
    connect(edit_currframe, SIGNAL(editingFinished()), this, SLOT(onCurrFrameEditingFinished()));

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
    edit_fps->setDisabled(true);

    // 播放/暂停按钮
    this->btn_play = new QPushButton();
    btn_play->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_play->setFixedHeight(SINGLE_LINE_HEIGHT);
    btn_play->setText(btnPlayText());
    btn_play->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(btn_play, SIGNAL(pressed()), this, SLOT(onClickPlay()));

    // 停止播放按钮
    this->btn_stop = new QPushButton();
    btn_stop->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_stop->setFixedHeight(SINGLE_LINE_HEIGHT);
    btn_stop->setText("stop");
    btn_stop->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(btn_stop, SIGNAL(pressed()), this, SLOT(onClickStop()));

    // 三连Key帧
    QPushButton *btn_key = new QPushButton();
    btn_key->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_key->setFixedHeight(SINGLE_LINE_HEIGHT);
    btn_key->setText("key 3");
    btn_key->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(btn_key, SIGNAL(pressed()), this, SLOT(onClickKeyThree()));

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
    layout->addWidget(btn_key);

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
    // timeline->setFixedWidth(timeline_width);
    // 绑定滑动消息
    connect(timeline, SIGNAL(valueChanged(int)), sliders_mapper, SLOT(map()));
    sliders_mapper->setMapping(timeline, QString(name.c_str()));
    sliders[name] = timeline;
    // K帧按钮
    QPushButton *btn_key = new QPushButton();
    btn_key->setText("key");
    btn_key->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_key->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    connect(btn_key, SIGNAL(pressed()), keys_mapper, SLOT(map()));
    keys_mapper->setMapping(btn_key, QString(name.c_str()));

    layout->addWidget(label_righthand);
    layout->addWidget(timeline);
    // layout->addStretch();
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
    btn_save->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    btn_save->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_save->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_save, SIGNAL(pressed()), this, SLOT(onClickBtnSave()));

    // 一句话
    QLineEdit *edit_sentence = new QLineEdit();
    edit_sentence->setFocusPolicy(Qt::ClickFocus);
    edit_sentence->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);
    edit_sentence->setFixedHeight(SINGLE_LINE_HEIGHT);
    edit_sentence->setPlaceholderText("edit sentence");

    // 播放
    QPushButton *btn_play = new QPushButton();
    btn_play->setText("sentence");
    btn_play->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    btn_play->setFixedWidth(SINGLE_BTN_WIDTH);
    btn_play->setFixedHeight(SINGLE_LINE_HEIGHT);
    connect(btn_play, SIGNAL(pressed()), this, SLOT(onClickBtnPlaySentence()));

    layout->addWidget(file_path);
    layout->addWidget(file_name);
    layout->addWidget(btn_save);
    layout->addStretch();
    layout->addWidget(edit_sentence);
    layout->addWidget(btn_play);

    return layout;
}

QScrollArea *QUIAnim::InitSignsScroll() {
    QScrollArea *scroll_area = new QScrollArea();

    this->scroll_content = new QWidget();
    this->scroll_layout = new QGridLayout();

    scroll_layout->setAlignment(Qt::AlignTop);
    scroll_content->setLayout(scroll_layout);
    scroll_content->adjustSize();

    scroll_area->setWidget(scroll_content);
    scroll_area->setWidgetResizable(true);
    // scroll_area->setFixedWidth(ADJUST_OPERATIONG_BTN_WIDTH);

    this->UpdateSignsScrollContent();

    return scroll_area;
}

void QUIAnim::UpdateSignsScrollContent() {
    const std::map<signs::SignId, std::string> &sign_dir = signer_->GetSavedSignDir();
    for (int i = 0; i < signs_btns.size(); ++i) {
        scroll_layout->removeWidget(signs_btns[i]);
        signs_mapper->removeMappings(signs_btns[i]);
        delete signs_btns[i];
    }
    signs_btns.clear();

    if (sign_dir.size() <= 0) return;

    if (nullptr == scroll_layout || nullptr == scroll_content) return;

    signs_btns.resize(sign_dir.size());
    std::map<signs::SignId, std::string>::const_iterator it = sign_dir.begin();
    int i = 0;
    while (it != sign_dir.end()) {
        const signs::SignId &id = it->first;

        QPushButton *btn = new QPushButton();
        btn->setText(QString("%1").arg(id));
        btn->setFixedHeight(SINGLE_BTN_HEIGHT);
        btn->setFixedWidth(2 * SINGLE_BTN_HEIGHT);
        btn->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

        // 绑定按钮消息
        connect(btn, SIGNAL(pressed()), signs_mapper, SLOT(map()));
        signs_mapper->setMapping(btn, id);

        scroll_layout->addWidget(btn, i / 17, i % 17);

        signs_btns[i] = btn;

        i++;
        it++;
    }
}

void QUIAnim::UpdateModel(const int &frame) {
    int at_frame = frame == -1 ? this->curr_frame : frame;
    for (models_map::iterator it = models_.begin(); it != models_.end(); ++it) {
        this->UpdateModel(it->first, calcThetas(it->first, at_frame));
    }
}

void QUIAnim::UpdateModel(const std::string &name, const smodel::Thetas &thetas) {
    models_[name]->Move(thetas);
    models_[name]->Update();
}

void QUIAnim::UpdateGL() {
    QUIManager::Instance().UpdateGL();
}

std::vector<int> QUIAnim::getKeyFrames(const std::string &name) {
    std::vector<int> key_frames;
    for (const std::pair<int, smodel::Thetas> &keypoint : keypoints[name]) {
        key_frames.push_back(keypoint.first);
    }
    return key_frames;
}

const smodel::Thetas QUIAnim::calcThetas(const std::string &name, const int &curr_frame) {
    std::vector<int> keyframes = getKeyFrames(name);
    smodel::ModelCtrl *model = models_[name];
    std::map<int, smodel::Thetas> &key_points = keypoints[name];
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


void QUIAnim::ConverSignDataToKeypoints(signs::SignData &sign_data, const int &start) {
    int frame_count = (int)sign_data.FrameCount();
    for (signs::Frame i = 0; i < frame_count; ++i) {
        models_map::iterator it = models_.begin();
        while (it != models_.end()) {
            signs::FrameParams params = sign_data.GetParams(it->first, i);
            if (params.size() > 0) {
                keypoints[it->first][i + start] = params;
            }
            it++;
        }
    }
}

/// >> SLOT 事件函数


void QUIAnim::onMaxFrameEditingFinished() {
    int max_frame_new = edit_maxframe->text().toInt();
    if (max_frame_new > 0) {
        this->max_frame = max_frame_new;
        std::map<std::string, QSlider *>::iterator it = sliders.begin();
        while (it != sliders.end()) {
            it->second->setMaximum(this->max_frame);
            it++;
        }
    } else {
        edit_maxframe->setText(QString("%1").arg(max_frame));
    }
}

void QUIAnim::onCurrFrameEditingFinished() {
    int curr_frame_new = edit_currframe->text().toInt();
    if (curr_frame_new > 0 && curr_frame_new <= this->max_frame) {
        this->curr_frame = curr_frame_new;
        this->updateTimer();
    } else {
        edit_currframe->setText(QString("%1").arg(curr_frame));
    }
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

    this->UpdateModel();
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

void QUIAnim::onClickKeyThree() {
    for (models_map::iterator it = models_.begin(); it != models_.end(); it++) {
        this->onClickKey(QString("%1").arg(it->first.c_str()));
    }
}

void QUIAnim::updateTimer() {
    if (this->curr_frame <= this->max_frame) {
        std::map<std::string, QSlider *>::iterator it = sliders.begin();
        if (it != sliders.end()) it->second->setValue(curr_frame);
        ++curr_frame;
    } else {
        this->onClickPlay();
    }
}

void QUIAnim::onClickBtnSave() {
    if (!file_path->text().isEmpty() && !file_name->text().isEmpty()) {
        const std::string file = (file_path->text() + file_name->text()).toStdString();
        std::map<std::string, std::map<int, smodel::Thetas>>::iterator it = keypoints.begin();
        int max = -1;
        int min = max_frame;
        while (it != keypoints.end()) {
            const std::string &model_name = it->first;
            std::vector<int> key_frames = getKeyFrames(model_name);
            if (key_frames.size() > 0) {
                max = MAX(key_frames[key_frames.size() - 1], max); // OpenCV
                min = MIN(key_frames[0], min);
            }
            it++;
        }

        std::ofstream fout;
        try {
            fout.open(file, std::ios::app | std::ios::out);
            if (fout.is_open()) {
                fout << "frame," << SIGNER_BODY_SAVED_HEADER << "," << SIGNER_RHAND_SAVED_HEADER << "," << SIGNER_LHAND_SAVED_HEADER << "\n";
                for (int frame = min; frame <= max; frame++) {
                    fout << frame - min + 1 << ",";
                    std::map<std::string, std::map<int, smodel::Thetas>>::iterator it_thetas = keypoints.begin();
                    std::map<std::string, smodel::Thetas> saved_data;
                    while (it_thetas != keypoints.end()) {
                        const std::string &model_name = it_thetas->first;
                        if (model_name == BODY) {
                            saved_data[SIGNER_BODY_SAVED_HEADER] = this->calcThetas(model_name, frame);
                        }
                        else if (model_name == RIGHT_HAND) {
                            saved_data[SIGNER_RHAND_SAVED_HEADER] = this->calcThetas(model_name, frame);
                        }
                        else if (model_name == LEFT_HAND) {
                            saved_data[SIGNER_LHAND_SAVED_HEADER] = this->calcThetas(model_name, frame);
                        }
                        it_thetas++;
                    }
                    int i = 1;
                    size_t size = saved_data[SIGNER_BODY_SAVED_HEADER].size();
                    for (const float &v : saved_data[SIGNER_BODY_SAVED_HEADER]) {
                        i++ < size ? fout << v << "#" : fout << v << ",";
                    }
                    i = 1;
                    size = saved_data[SIGNER_RHAND_SAVED_HEADER].size();
                    for (const float &v : saved_data[SIGNER_RHAND_SAVED_HEADER]) {
                        i++ < size ? fout << v << "#" : fout << v << ",";
                    }
                    i = 1;
                    size = saved_data[SIGNER_LHAND_SAVED_HEADER].size();
                    for (const float &v : saved_data[SIGNER_LHAND_SAVED_HEADER]) {
                        i++ < size ? fout << v << "#" : fout << v << "\n";
                    }
                }
                QMessageBox::information(this, tr("Success"), tr("Save Success!"), QMessageBox::Yes);
                this->signer_->ReloadSignsDir();
                this->UpdateSignsScrollContent();
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
    }
    else {
        QMessageBox::warning(this, tr("Error"), tr("Please fill the path & id"), QMessageBox::Yes);
    }
}

void QUIAnim::onClickSignsBtn(const int &id) {
    signs::SignData sign_data = this->signer_->GetSignData((signs::SignId)id);
    if (sign_data.IsValid()) {
        QMessageBox::StandardButton res = QMessageBox::question(this, tr("Question"), tr("Replace the animation?"), QMessageBox::Yes | QMessageBox::Cancel);
        if (res == QMessageBox::Yes) {
            keypoints.clear();
            this->curr_frame = 0;
            this->max_frame = (int)sign_data.FrameCount();

            this->ConverSignDataToKeypoints(sign_data, 0);

            edit_maxframe->setText(QString("%1").arg(max_frame));
            this->onMaxFrameEditingFinished();
            edit_currframe->setText(QString("%1").arg(curr_frame));
            this->onCurrFrameEditingFinished();

            this->UpdateModel();
            this->UpdateGL();
            this->onClickPlay();
        }
    }
}

void QUIAnim::onClickBtnPlaySentence() {
    keypoints.clear();
    this->curr_frame = 0;
    this->max_frame = 0;

    signs::SignDatas sign_datas = signer_->Animate("");
    signs::SignDatas::iterator it = sign_datas.begin();
    while (it != sign_datas.end()) {
        this->ConverSignDataToKeypoints(*it, this->max_frame);

        this->max_frame += it->FrameCount();
        this->max_frame += 30;

        it++;
    }

    edit_maxframe->setText(QString("%1").arg(max_frame));
    this->onMaxFrameEditingFinished();
    edit_currframe->setText(QString("%1").arg(curr_frame));
    this->onCurrFrameEditingFinished();

    this->UpdateModel();
    this->UpdateGL();
    this->onClickPlay();
}
