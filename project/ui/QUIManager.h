#ifndef QUI_MANAGER_H
#define QUI_MANAGER_H

#include "QUIConst.h"
#include "GLWindow.h"
#include "pannels/QUIAnim.h"
#include "pannels/QUIDashboard.h"
#include "pannels/QUIQuick.h"
#include "pannels/QUIRenderImg.h"

namespace ui {

class QUIManager {
private:
    GLWindow *gl_window;
    QUIQuick *ui_quick;
    QUIDashboard *ui_body;
    QUIDashboard *ui_right_hand;
    QUIDashboard *ui_left_hand;
    QUIAnim *ui_anim;
    QUIRenderImg *ui_render_img;

    std::string name;
    int center_id;

    models_map models;
    std::string project_path_;

private:
    QUIManager();

public:
    QUIManager(const QUIManager &) = delete;
    QUIManager &operator=(const QUIManager&) = delete;
    ~QUIManager();
    static QUIManager &Instance();

public:
    void Init();
    void Show();
    void UpdateGL();
    void SetProjectPath(const std::string &path) { this->project_path_ = path; }
    void SetGLWindow(GLWindow *window) { gl_window = window; }
    inline void AddModel(smodel::ModelCtrl &model) { models[model.GetName()] = &model; }
    smodel::ModelCtrl *GetModel(const std::string &name);
    models_map &GetModels();
    inline GLWindow *GetGLWindow() const { return gl_window; }
    QUIQuick *GetUIQuick();

public:
    void ShowQuickPannel();
    void ShowBodyPannel();
    void ShowRightHandPannel();
    void ShowLeftHandPannel();
    void ShowAnimPannel();
    void ShowRenderImgPannel();
    void SetSelected(const std::string &name, const int &center_id);
    const std::string &GetSelectedModel() const;
    const int &GetSelectedCenterId() const;
    void SetRendererState(const int &id, const bool &is_render);
};

}
// namespace ui

#endif // QUI_MANAGER_H
