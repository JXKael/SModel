#include <QApplication>
#include "model/ModelCtrl.h"
#include "ui/QUIManager.h"

int main(int argc, char *argv[]) {
    const std::string project_path = "E:/Code/GitHub/SModel/project";

    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);

    // 身体
    smodel::ModelCtrl body(BODY, project_path);
    body.Init();

    // 右手
    smodel::ModelCtrl right_hand(RIGHT_HAND, project_path);
    right_hand.Init();

    // 左手
    smodel::ModelCtrl left_hand(LEFT_HAND, project_path);
    left_hand.Init();

    body.AddChild(right_hand, 6);
    body.AddChild(left_hand, 9);

    ui::GLWindow window;
    window.resize(GL_WINDOW_WIDTH, GL_WINDOW_HEIGHT);
    window.SetProjectPath(project_path);

    ui::QUIManager::Instance().SetProjectPath(project_path);
    ui::QUIManager::Instance().SetGLWindow(&window);
    ui::QUIManager::Instance().AddModel(body);
    ui::QUIManager::Instance().AddModel(right_hand);
    ui::QUIManager::Instance().AddModel(left_hand);

    ui::QUIManager::Instance().Init();
    ui::QUIManager::Instance().Show();

    return app.exec();
}
