#ifndef QUI_QUICK_H
#define QUI_QUICK_H

#include "QUIConst.h"
#include "utils/csv.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QSignalMapper>
#include <QMessageBox>
#include <climits>

namespace ui {

class QUIPannelCtrl;

typedef std::pair<std::string, smodel::Thetas> quick_theta_pair;
typedef std::vector<quick_theta_pair> quick_thetas_list;

class QUIQuick: public QDialog {
    Q_OBJECT
public:
    explicit QUIQuick(QWidget *patent = 0);
    explicit QUIQuick(const std::string &project_path, models_map &models, renderers_map &renderers);
    ~QUIQuick();

private:
    const std::string project_path_;
    models_map &models_;
    renderers_map &renderers_;

    std::string sel_name;
    // 初始参数
    std::map<std::string, std::vector<float>> init_thetas;
    // 快捷参数
    quick_thetas_list quick_thetas;
    // 复选框
    std::map<int, QCheckBox *> check_boxes;
    // 滑动框
    QWidget *scroll_content;
    QGridLayout *scroll_layout;
    std::vector<QPushButton *> quick_btns;

    // 转发器
    QSignalMapper *quick_mapper;
    QSignalMapper *checkbox_mapper;
    QSignalMapper *checkbox_btn_mapper;

public:
    void Init();
    void UpdateModel(const std::string &name, const smodel::Thetas &thetas);
    void UpdateGL();
    void ApplyModelAs(const std::string &name, const smodel::Thetas &thetas);

private:
    QVBoxLayout *InitCheckBoxes();
    void LoadQuickThetas(const std::string &name);
    QVBoxLayout *InitQuickThetas(const std::string &name);
    QScrollArea *InitQuickScroll(const std::string &name);
    void UpdateQuickScrollContent(const std::string &name);
    void ConnectMapper();
    smodel::Thetas convertToThetas(const std::string &thetas_s);

private Q_SLOTS:
    void onCheckBoxToggled(int id);
    void onModelBtnClick(const QString &name);
    void onQuickBtnClick(int id);
    void onClickBtnReset();
};

}
// namespace ui

#endif // QUI_QUICK_H
