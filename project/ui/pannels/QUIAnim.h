#ifndef QUI_ANIM_H
#define QUI_ANIM_H

#include "../QUIConst.h"

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

namespace ui {

class QUIAnim : public QDialog {
    Q_OBJECT
public:
    explicit QUIAnim(QWidget *patent = 0);
    QUIAnim(const std::string &project_path, std::map<std::string, smodel::ModelCtrl *> &models);
    ~QUIAnim();

private:
    const std::string project_path_;
    std::map<std::string, smodel::ModelCtrl *> &models_;

    std::map<std::string, std::map<int, smodel::Thetas>> keypoints;

    // animation
    const int fps = 60;
    const int timeline_width = 1200;
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

    QSignalMapper *sliders_mapper;
    QSignalMapper *keys_mapper;

public:
    void Init();
    void UpdateModel(const std::string &name, const smodel::Thetas &thetas);
    void UpdateGL();

private:
    QHBoxLayout *InitHead();
    QHBoxLayout *InitTimeline(const std::string &name, const smodel::ModelCtrl *model);
    QHBoxLayout *InitOperation();
    inline bool isPlaying() { return this->timer->isActive(); }
    inline char *btnPlayText() { return this->isPlaying() ? "pause" : "play"; }
    const smodel::Thetas calcThetas(const std::string &name);

private Q_SLOTS:
    void onClickPlay();
    void onClickStop();
    void onSliderValueChanged(const QString &name);
    void onClickKey(const QString &name);
    void updateTimer();
    void onClickBtnSave();
};
// class QUIAnim

}
// namespace ui

#endif // QUI_ANIM_H
