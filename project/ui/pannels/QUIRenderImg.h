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

namespace ui {

class QUIRenderImg : public QDialog {
    Q_OBJECT
public:
    explicit QUIRenderImg(QWidget *patent = 0);
    explicit QUIRenderImg(const std::string &project_path);
    ~QUIRenderImg();

private:
    const std::string project_path_;

    QLineEdit *file_path;
    QLineEdit *file_name;
    QLabel *file_suffix;

public:
    void Init();
    void UpdateModel(const std::string &name, const smodel::Thetas &thetas);
    void UpdateGL();

private Q_SLOTS:
    void onClickBtnRenderHand();
    void onClickBtnRender();
    void onClickBtnRenderBatch();
};
// class QUIRenderImg

}
// namespace ui

#endif // QUI_RENDER_IMG_H
