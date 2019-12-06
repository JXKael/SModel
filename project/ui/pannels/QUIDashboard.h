#ifndef QUI_BODY_PANNEL_H
#define QUI_BODY_PANNEL_H

#include "../QUIConst.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QPushButton>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSignalMapper>
#include <QMessageBox>
#include <climits>

namespace ui {

class QUIDashboard: public QDialog {
    Q_OBJECT
public:
    explicit QUIDashboard(QWidget *patent = 0);
    explicit QUIDashboard(smodel::ModelCtrl *model_ctrl);
    ~QUIDashboard();

private:
    smodel::ModelCtrl *model_ctrl_;

    // 初始参数
    std::vector<float> init_thetas_;

    // 滑动条
    std::vector<QSlider *> sliders;
    // 编辑框
    std::vector<QLineEdit *> line_edits;
    // 转发器
    QSignalMapper *s2e_mapper;
    QSignalMapper *e2s_mapper;
    // 滑动条映射参数，thetas' idx ->( K or B -> val)
    std::vector<std::map<ParamType, float>> linear_map;
    // 自由度
    std::vector<std::map<MINMAX, float>> dofs_;

    // 结构树
    QTreeWidget *tree;
    int selected_boneid = -1;
    int selected_centerid = -1;

public:
    void Init();
    void UpdateModel(const smodel::Thetas &thetas);
    void UpdateGL();
    void ApplyAs(const smodel::Thetas &thetas);
private:
    void InitDofs();
    QGridLayout *InitThetasPannel();
    QGridLayout *InitTreePannel();
    void AddBoneTreeItem(const smodel::Bone &bone, QTreeWidgetItem *parentItem);
    void AddAttachCenters(const smodel::Bone &bone, QTreeWidgetItem *parentItem);
    void ConnectMapper();
    void ClampTheta(float *theta, int *slider_val, float min, float max);

private slots :
    void onClickBtnPick();
    void sliderHandler(int param);
    void editHandler(int param);
    void onTreeItemClicked(QTreeWidgetItem *item, int column);
};

}
// namespace ui

#endif // QUI_BODY_PANNEL_H
