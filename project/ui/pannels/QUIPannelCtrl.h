#ifndef QUI_PANNEL_CTRL_H
#define QUI_PANNEL_CTRL_H

#include "model/ModelCtrl.h"
#include "QUIDashboard.h"

namespace ui {

class GLWindow;

class QUIPannelCtrl {
private:
    GLWindow *gl_window_;
    QUIDashboard *ui_body;
    QUIDashboard *ui_right_hand;
    QUIDashboard *ui_left_hand;
public:
    QUIPannelCtrl(GLWindow *gl_window);
    ~QUIPannelCtrl();
    void UpdateGL();
    void ShowBodyPannel(smodel::ModelCtrl *body_ctrl);
    void ShowRightHandPannel(smodel::ModelCtrl *body_ctrl);
    void ShowLeftHandPannel(smodel::ModelCtrl *body_ctrl);
};

}
// namespace ui

#endif // QUI_PANNEL_CTRL_H