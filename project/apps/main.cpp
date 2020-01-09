#include <QApplication>
#include "model/ModelsManager.h"
#include "ui/QUIManager.h"
#include "signer/Signer.h"

int main(int argc, char *argv[]) {
    const std::string project_path = "E:/Code/GitHub/SModel/project";
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }

    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);

    // 模型
    smodel::ModelsManager models_mgr(project_path + "/");
    models_mgr.Init();

    // 动画
    signs::Signer signer(project_path);

    ui::QUIManager::Instance().SetProjectPath(project_path);
    ui::QUIManager::Instance().SetModelsMgr(models_mgr);
    ui::QUIManager::Instance().SetSigner(signer);

    ui::QUIManager::Instance().Init();
    ui::QUIManager::Instance().Show();

    return app.exec();
}
