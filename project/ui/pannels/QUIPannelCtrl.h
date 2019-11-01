#ifndef QUI_PANNEL_CTRL_H
#define QUI_PANNEL_CTRL_H

#include "model/ModelCtrl.h"
#include "QUIDashboard.h"
#include "QUIAnim.h"

namespace ui {

class GLWindow;

class QUIPannelCtrl {
private:
    GLWindow *gl_window_;
    QUIDashboard *ui_body;
    QUIDashboard *ui_right_hand;
    QUIDashboard *ui_left_hand;
    QUIAnim *ui_anim;

    std::string name;
    int center_id;
public:
    QUIPannelCtrl(GLWindow *gl_window);
    ~QUIPannelCtrl();
    void UpdateGL();
    void ShowAnimPannel(std::map<std::string, smodel::ModelCtrl *> &models);
    void ShowBodyPannel(smodel::ModelCtrl *body_ctrl);
    void ShowRightHandPannel(smodel::ModelCtrl *body_ctrl);
    void ShowLeftHandPannel(smodel::ModelCtrl *body_ctrl);
    void SetSelected(const std::string &name, const int &center_id);
    const std::string &GetSelectedModel() const;
    const int &GetSelectedCenterId() const;
};

}
// namespace ui

#endif // QUI_PANNEL_CTRL_H