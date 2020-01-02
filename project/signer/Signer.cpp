#include "Signer.h"

using namespace signs;

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

void Signer::ReloadSignsDir() {
    this->sign_loader->ReloadSignsDir();
}

const std::map<SignId, std::string> &Signer::GetSavedSignDir() const {
    return sign_loader->GetSavedSignDir();
}

SignDatas Signer::Animate(const Glosses &glosses) {
    SignDatas sign_datas = sign_loader->Load(glosses);
    return sign_datas;
}
