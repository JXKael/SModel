#ifndef QUI_MANAGER_H
#define QUI_MANAGER_H

#include "QUIConst.h"
#include "MainWindow.h"
#include "GLWidget.h"
#include "pannels/QUIAnim.h"
#include "pannels/QUIDashboard.h"
#include "pannels/QUIQuick.h"
#include "pannels/QUIRenderImg.h"
#include "pannels/QUIPointCloud.h"

#include "../model/ModelsManager.h"
#include "../signer/Signer.h"

namespace ui {

#define GL_WINDOW_WIDTH 1200
#define GL_WINDOW_HEIGHT 675

#define MAIN_WIN_WIDTH 1200
#define MAIN_WIN_HEIGHT 675

class QUIManager {
private:
    MainWindow *main_win;
    GLWidget *gl_widget;
    QUIQuick *ui_quick;
    QUIDashboard *ui_body;
    QUIDashboard *ui_right_hand;
    QUIDashboard *ui_left_hand;
    QUIAnim *ui_anim;
    QUIRenderImg *ui_render_img;
    QUIPointCloud *ui_point_cloud;

    std::string name;
    int center_id;

    smodel::models_map models;
    std::string project_path_;

    signs::Signer *signer;
    smodel::ModelsManager *models_mgr;

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
    inline void AddModel(smodel::ModelCtrl &model) { models[model.GetName()] = &model; }
    smodel::ModelCtrl *GetModel(const std::string &name);
    smodel::models_map &GetModels();
    inline GLWidget *GetGLWindow() const { return gl_widget; }
    QUIQuick *GetUIQuick();

    inline const smodel::ModelsManager *GetModelsMgr() const { return this->models_mgr; }
    inline void SetModelsMgr(smodel::ModelsManager &models_mgr) { this->models_mgr = &models_mgr; }
    inline void SetSigner(signs::Signer &signer) { this->signer = &signer; }
    inline const signs::Signer *GetSigner() const { return this->signer; }

public:
    void ShowQuickPannel();
    void ShowBodyPannel();
    void ShowRightHandPannel();
    void ShowLeftHandPannel();
    void ShowAnimPannel();
    void ShowRenderImgPannel();
    void ShowPointCloudWindow(const std::string &file_path);
    void SetSelected(const std::string &name, const int &center_id);
    const std::string &GetSelectedModel() const;
    const int &GetSelectedCenterId() const;
    void SetRendererState(const int &id, const bool &is_render);
};

}
// namespace ui

#endif // QUI_MANAGER_H
