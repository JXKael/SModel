#include <QApplication>
#include <ui/GLWindow.h>
#include "model/ModelCtrl.h"

int main(int argc, char *argv[]) {
    const std::string project_path = "E:/Code/GitHub/SModel/project";

    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    ui::GLWindow window;
    window.resize(1600, 900);
    window.SetProjectPath(project_path);

    // 身体
    smodel::ModelCtrl body(project_path + "/data/body");
    body.Init();

    // 右手
    smodel::ModelCtrl right_hand(project_path + "/data/right_hand");
    right_hand.Init();

    // 左手
    smodel::ModelCtrl left_hand(project_path + "/data/left_hand");
    left_hand.Init();

    body.AddChild(right_hand, 6);
    body.AddChild(left_hand, 9);

    window.AddModel("body", body);
    window.AddModel("RightHand", right_hand);
    window.AddModel("LeftHand", left_hand);

    window.show();

    return app.exec();
}
