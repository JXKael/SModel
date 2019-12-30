#include "Signer.h"

using namespace signer;

Signer::Signer() {
    this->init_data_loader();
}

Signer::Signer(const std::string &project_path): project_path_(project_path) {
    this->init_data_loader();
}

Signer::Signer(const std::string &project_path, models_map &models)
  : project_path_(project_path),
    models_(models)
{
    this->init_data_loader();
}

void Signer::init_data_loader() {
    std::unique_ptr<SignDataLoader> sign_data_loader(new SignDataLoader("E:/Code/GitHub/SModel/project/data/signs"));
    sign_loader = std::move(sign_data_loader);
}

Signer::~Signer() {
    models_.clear();
}

void Signer::Animate(Glosses &glosses) {
    Glosses::iterator it = glosses.begin();
    while (it != glosses.end()) {
        SignData sign_data = sign_loader->Load(*it);

        ++it;
    }
}
