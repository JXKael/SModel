#ifndef QUI_QUICK_H
#define QUI_QUICK_H

#include "../QUIConst.h"

#include <fstream>

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

class QUIQuick: public QDialog {
    Q_OBJECT
public:
    explicit QUIQuick(QWidget *patent = 0);
    QUIQuick(const std::string &project_path, smodel::models_map &models, renderers_map &renderers, renderers_state_map &renderers_state);
    ~QUIQuick();

private:
    const std::string project_path_;
    smodel::models_map &models_;
    renderers_map &renderers_;
    renderers_state_map &renderers_state_;

    // 当前选择的模型的名称
    std::string sel_model_name;
    // 初始参数
    std::map<std::string, std::vector<float>> init_thetas;
    // 快捷参数
    quick_thetas_list quick_thetas;
    // 左上按钮
    std::map<std::string, QPushButton *> model_btn;
    // 渲染器复选框
    std::map<int, QCheckBox *> check_boxes;
    // 滑动框
    QWidget *scroll_content;
    QGridLayout *scroll_layout;
    std::vector<QPushButton *> quick_btns;
    QPushButton *btn_reset;
    QPushButton *btn_save;
    QLineEdit *edit_pose_name;
    QLineEdit *edit_mask_val;

    // 转发器
    QSignalMapper *quick_mapper;
    QSignalMapper *checkbox_mapper;
    QSignalMapper *checkbox_btn_mapper;

    // 存储当前模型的姿势名称, model_name -> pose_name
    std::map<std::string, std::string> model_pose;
    //model_name -> pose_name -> mask_val
    std::map<std::string, std::map<std::string, int>> pose_mask_map;

public:
    void Init();
    void UpdateModel(const std::string &model_name, const smodel::Thetas &thetas);
    void UpdateGL();
    void ApplyModelAs(const std::string &model_name, const smodel::Thetas &thetas);
    std::map<std::string, int> GetModelMaskVals();
    quick_thetas_list &GetQuickThetas();

private:
    QVBoxLayout *InitCheckBoxes();
    void LoadQuickThetas(const std::string &model_name);
    QVBoxLayout *InitQuickThetas(const std::string &model_name);
    QScrollArea *InitQuickScroll(const std::string &model_name);
    void UpdateQuickScrollContent(const std::string &model_name);
    void ConnectMapper();
    smodel::Thetas convertToThetas(const std::string &thetas_s);

private Q_SLOTS:
    void onCheckBoxToggled(int id);
    void onModelBtnClick(const QString &name);
    void onQuickBtnClick(int id);
    void onClickBtnSave();
    void onClickBtnReset();
    void onClickBtnRenderImg();
};

}
// namespace ui

#endif // QUI_QUICK_H
