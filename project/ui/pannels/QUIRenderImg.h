#ifndef QUI_RENDER_IMG_H
#define QUI_RENDER_IMG_H

#include "QUIConst.h"

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
    QUIRenderImg(const std::string &project_path, models_map &models);
    ~QUIRenderImg();

private:
    const std::string project_path_;
    models_map &models_;

    QLineEdit *file_path;
    QLineEdit *file_name;
    QLabel *file_suffix;

    std::map<std::string, int> pose_mask_map;

public:
    void Init();
    void UpdateModel(const std::string &name, const smodel::Thetas &thetas);
    void UpdateGL();

private Q_SLOTS:
    void onClickBtnSetRenderHand();
    void onClickBtnSetRenderMask();
    void onClickBtnRenderResetScreen();

    void onClickBtnRenderOne();
    void onClickBtnRenderBatch();

    void onClickBtnRenderMaskOne();
    void onClickBtnRenderMaskBatch();

    void onClickBtnOneForAll();
    void onClickBtnOneForAllMask();
};
// class QUIRenderImg

}
// namespace ui

#endif // QUI_RENDER_IMG_H
