#ifndef MODELS_MANAGER_H
#define MODELS_MANAGER_H

// #include "ModelCtrl.h"
#include "ModelConfigLoader.h"

#include <string>
#include <map>

namespace smodel {

// composed model name -> model_ctrls
typedef std::map<std::string, ModelCtrlMap> ComposedModels;

class ModelsManager {
    const std::string project_dir_;
    ModelConfigLoader config_loader;

    ComposedModels composed_models;

public:
    // constructor & destructor
    ModelsManager();
    explicit ModelsManager(const std::string &project_dir);
    ModelsManager(const ModelsManager &models_manager) = delete;
    ~ModelsManager();

    void Init();

    inline ComposedModels &GetAllModels() { return composed_models; }

private:
    void loadModels();
    void loadModelConfig(ModelCtrl *model_ctrl);
    void initHierarch();
};

} // namespace smodel

#endif // MODELS_MANAGER_H
