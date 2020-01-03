#ifndef QUI_ANIM_H
#define QUI_ANIM_H

#include "../QUIConst.h"
#include "../../signer/Signer.h"

#include <QDialog>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QSignalMapper>
#include <QTimer>
#include <QGridLayout>
#include <QScrollArea>
#include <QMessageBox>

#include <fstream>

namespace ui {

class QUIAnim : public QDialog {
    Q_OBJECT
public:
    explicit QUIAnim(QWidget *patent = 0);
    QUIAnim(const std::string &project_path, models_map &models, signs::Signer *signer);
    ~QUIAnim();

private:
    const std::string project_path_;
    models_map &models_;
    signs::Signer *signer_;

    std::map<std::string, std::map<int, smodel::Thetas>> keypoints;

    // animation
    const int fps = 60;
    int max_frame;
    int curr_frame;
    QTimer *timer;

    // UI
    QLineEdit *edit_maxframe;
    QLineEdit *edit_currframe;
    QLineEdit *edit_fps;
    QPushButton *btn_play;
    QPushButton *btn_stop;
    std::map<std::string, QSlider *> sliders;
    QLineEdit *file_path;
    QLineEdit *file_name;
    // 滑动框
    QWidget *scroll_content;
    QGridLayout *scroll_layout;
    std::vector<QPushButton *> signs_btns;

    QSignalMapper *sliders_mapper;
    QSignalMapper *keys_mapper;
    QSignalMapper *signs_mapper;

public:
    void Init();
    void UpdateModel(const int &frame = -1);
    void UpdateModel(const std::string &name, const smodel::Thetas &thetas);
    void UpdateGL();

private:
    QHBoxLayout *InitHead();
    QHBoxLayout *InitTimeline(const std::string &name, const smodel::ModelCtrl *model);
    QHBoxLayout *InitOperation();
    QScrollArea *InitSignsScroll();
    void UpdateSignsScrollContent();
    inline bool isPlaying() { return this->timer->isActive(); }
    inline char *btnPlayText() { return this->isPlaying() ? "pause" : "play"; }
    std::vector<int> getKeyFrames(const std::string &name);
    const smodel::Thetas calcThetas(const std::string &name, const int &curr_frame);
    void ConverSignDataToKeypoints(signs::SignData &sign_data, const int &start);

private Q_SLOTS:
    void onMaxFrameEditingFinished();
    void onCurrFrameEditingFinished();
    void onClickPlay();
    void onClickStop();
    void onSliderValueChanged(const QString &name);
    void onClickKey(const QString &name);
    void onClickKeyThree();
    void updateTimer();
    void onClickBtnSave();
    void onClickSignsBtn(const int &id);
    void onClickBtnPlaySentence();
};
// class QUIAnim

}
// namespace ui

#endif // QUI_ANIM_H
