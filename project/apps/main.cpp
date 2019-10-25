#include <QApplication>
#include <ui/GLWindow.h>
#include "model/ModelCtrl.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    ui::GLWindow window;
    window.resize(1600, 900);

    std::string project_path = "E:/Code/C/SModel/project";

    // 坐标轴渲染
    QString vshader((project_path + "/ui/shaders/simple3D_vs.glsl").c_str());
    QString fshader((project_path + "/ui/shaders/simple3D_fs.glsl").c_str());
    ui::FixAxisRenderer axis_renderer(vshader, fshader);

    // 身体
    smodel::ModelCtrl body(project_path + "/data/body");
    body.Init();

    ui::ConvolutionRenderer body_renderer(project_path);
    body_renderer.SetRenderModel(body);

    // 右手
    smodel::ModelCtrl right_hand(project_path + "/data/right_hand");
    right_hand.Init();

    ui::ConvolutionRenderer rhand_renderer(project_path);
    rhand_renderer.SetRenderModel(right_hand);

    // 左手
    smodel::ModelCtrl left_hand(project_path + "/data/left_hand");
    left_hand.Init();

    ui::ConvolutionRenderer lhand_renderer(project_path);
    lhand_renderer.SetRenderModel(left_hand);

    body.AddChild(right_hand, 6);
    body.AddChild(left_hand, 9);

    window.AddModel("body", body);
    window.AddModel("RightHand", right_hand);
    window.AddModel("LeftHand", left_hand);

    window.AddRenderer(body_renderer);
    window.AddRenderer(rhand_renderer);
    window.AddRenderer(lhand_renderer);
    window.AddRenderer(axis_renderer);

    window.show();

    return app.exec();
}
