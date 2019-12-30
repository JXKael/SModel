#ifndef SIGNER_H
#define SIGNER_H

#include "SignDataLoader.h"
#include "model/ModelCtrl.h"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

namespace signer {

// model_name -> model_ctrl
typedef std::map<std::string, smodel::ModelCtrl *> models_map;

class Signer {

private:
    std::unique_ptr<SignDataLoader> sign_loader;
    std::string project_path_;
    models_map models_;

    void init_data_loader();

public:
    Signer();
    explicit Signer(const std::string &project_path);
    Signer(const std::string &project_path, models_map &models);

    ~Signer();

    inline void AddModel(smodel::ModelCtrl &model) { models_[model.GetName()] = &model; }

    void Animate(Glosses &glosses);
};

} // namespace singer

#endif // SIGNER_H
