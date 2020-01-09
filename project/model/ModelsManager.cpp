#include "ModelsManager.h"
//#include "../utils/SMLog.h"
#include "../utils/dir_utils.h"

using namespace smodel;

ModelsManager::ModelsManager() : project_dir_("") {

}

ModelsManager::ModelsManager(const std::string &project_dir): project_dir_(project_dir) {

}

ModelsManager::~ModelsManager() {
    ComposedModels::iterator it = composed_models.begin();
    while (it != composed_models.end()) {
        ModelCtrlMap &model_ctrls = it->second;
        ModelCtrlMap::iterator it_models = model_ctrls.begin();
        while (it_models != model_ctrls.end()) {
            ModelCtrl *model_ctrl = it_models->second;
            if (model_ctrl != nullptr)
                delete model_ctrl;
            model_ctrl = nullptr;
            it_models++;
        }
        model_ctrls.clear();
        it++;
    }
}

void ModelsManager::Init() {
    this->loadModels();
}

void ModelsManager::loadModels() {
    if (project_dir_.size() < 0) {
        //utils::SMLog::logger().print(utils::kError, "project directory is null!");
        return;
    }

    const std::string &models_dir = utils::get_models_dir(project_dir_);

    utils::FilesList files_list = utils::list_files(models_dir.c_str());
    for (const utils::FileName &file : files_list) {
        const std::string &name = file;
        ModelCtrlMap model_ctrls = config_loader.LoadComposedModel(models_dir + file);
        ModelCtrlMap::iterator it = model_ctrls.begin();
        while (it != model_ctrls.end()) {
            ModelCtrl *model_ctrl = it->second;
            this->loadModelConfig(model_ctrl);
            model_ctrl->Init();
            it++;
        }
        composed_models[name] = model_ctrls;
    }

    this->initHierarch();
}

void ModelsManager::loadModelConfig(ModelCtrl *model_ctrl) {
    const std::string &name = model_ctrl->GetName();
    const std::string &data_dir = utils::get_data_dir(project_dir_) + name + "/";
    Model *model = model_ctrl->GetModel();
    model->Clear();
    model->centers = config_loader.LoadCenters(data_dir + "centers.csv");
    model->bones = config_loader.LoadBones(data_dir + "bones.csv");
    model->blocks = config_loader.LoadBlocks(data_dir + "blocks.csv");
    model->block_colors = config_loader.LoadBlockColors(data_dir + "blocks.csv");
    model->dofs = config_loader.LoadDofs(data_dir + "dofs.csv");
}

void ModelsManager::initHierarch() {
    ComposedModels::iterator it = composed_models.begin();
    while (it != composed_models.end()) {
        ModelCtrlMap &model_ctrls = it->second;
        ModelCtrlMap::iterator it_models = model_ctrls.begin();
        while (it_models != model_ctrls.end()) {
            const int &id = it_models->first;
            ModelCtrl *model_ctrl = it_models->second;
            if (model_ctrl->HasParent()) {
                const int parent_id = model_ctrl->GetParentId();
                const int to_boneid = model_ctrl->GetParentBoneid();
                ModelCtrlMap::iterator it_parent = model_ctrls.find(parent_id);
                if (it_parent != model_ctrls.end()) {
                    ModelCtrl * parent = it_parent->second;
                    parent->AddChild(model_ctrl, to_boneid);
                }
                else {
                    std::cout << "### [error]: the parnet id [" << parent_id << "] is illegal." << std::endl;
                }
            }
            it_models++;
        }
        it++;
    }
}
