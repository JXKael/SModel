#include "Signer.h"

using namespace signs;

Signer::Signer() {
    this->init_data_loader();
}

Signer::Signer(const std::string &project_path): project_path_(project_path) {
    this->init_data_loader();
}

Signer::Signer(const std::string &project_path, smodel::models_map &models)
  : project_path_(project_path),
    models_(models)
{
    this->init_data_loader();
}

void Signer::init_data_loader() {
    std::unique_ptr<SignDataLoader> sign_data_loader(new SignDataLoader(project_path_ + "/data/signs"));
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

SignData Signer::GetSignData(const SignId &id) {
    return sign_loader->Load(id);
}

SignDatas Signer::Animate(const std::string &sentence) {
    signs::Glosses glosses;
    glosses.push_back(signs::LexicalItem("她")); // 168
    glosses.push_back(signs::LexicalItem("们")); // 370
    glosses.push_back(signs::LexicalItem("漂亮")); // 20
    return this->Animate(glosses);
}

SignDatas Signer::Animate(const Glosses &glosses) {
    SignDatas sign_datas = sign_loader->Load(glosses);
    return sign_datas;
}
