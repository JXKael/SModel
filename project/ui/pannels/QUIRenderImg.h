#ifndef QUI_RENDER_IMG_H
#define QUI_RENDER_IMG_H

#include "../QUIConst.h"
#include "../GLWidget.h"

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

#include <iomanip>

namespace ui {

class QUIRenderImg : public QDialog {
    Q_OBJECT
public:
    explicit QUIRenderImg(QWidget *patent = 0);
    QUIRenderImg(const std::string &project_path, models_map &models, GLWidget *gl_widget);
    ~QUIRenderImg();

private:
    const std::string project_path_;
    models_map &models_;
    GLWidget *gl_widget_;

    QLineEdit *file_path;
    QLineEdit *file_name;
    QLabel *file_suffix;

    std::map<std::string, quick_thetas_list> pose_thetas_map;
    std::map<std::string, quick_mask_list> pose_mask_map;

public:
    void Init();
    void UpdateModel(const std::string &name, const smodel::Thetas &thetas);
    void UpdateGL();

private:
    void loadQuickThetas(const std::string &project_path, const std::string &model_name, smodel::ModelCtrl *model_ctrl);

private Q_SLOTS:
    void onClickBtnNormScreen();
    void onClickBtnResetScreen();

    void onClickBtnRenderOne();
    void onClickBtnRenderBatch();

    void onClickBtnRenderMaskOne();
    void onClickBtnRenderMaskBatch();

    void onClickBtnRenderFingerMaskOne();
    void onClickBtnRenderFingerMaskBatch();

    void onClickBtnOneForAll();
    void onClickBtnOneForAllMask();
    void onClickBtnOneForAllFingerMask();
};
// class QUIRenderImg

}
// namespace ui

#endif // QUI_RENDER_IMG_H
