#include "SignDataLoader.h"

using namespace signs;

SignDataLoader::SignDataLoader() {

}

SignDataLoader::SignDataLoader(const std::string &root): root_(root) {
    Init();
}

SignDataLoader::~SignDataLoader() {
    root_.clear();
    signs_dir.clear();
    glosses_map.clear();
}

void SignDataLoader::Init() {
    if (root_.size() <= 0) {
        std::cout << "!!! Error: empty path of data, Initialization failed" << std::endl;
        return;
    }
    // map
    glosses_map.clear();
    std::string map_path = GetGlossesMapFilePath();
    try {
        csv::CSVReader<2> reader(map_path);
        reader.read_header(csv::ignore_extra_column, "gloss", "ids");
        std::string gloss, ids_s;
        while (reader.read_row(gloss, ids_s)) {
            SignIds ids = utils::ConvertToVectorInt(ids_s);
            glosses_map[gloss] = ids;
        }
    }
    catch (csv::error::can_not_open_file err) {
        std::cout << "!!! Error: Can't open file.\nDetail: " << err.what() << std::endl;
    }
    catch (...) {
        std::cout << "!!! Unknow Error while reading " << map_path << "." << std::endl;
    }

    // data
    this->ReloadSignsDir();
}

const std::string SignDataLoader::GetDataRoot() {
    return root_ + "/sign_data/";
}

const std::string SignDataLoader::GetGlossesMapFilePath() {
    return root_ + "/glosses_map/glosses_map.csv";
}

void SignDataLoader::ReloadSignsDir() {
    std::string data_root = GetDataRoot();

    std::vector<std::string> files = utils::listFiles(data_root.c_str());
    if (files.size() <= 0) {
        std::cout << "--> Warning: nothing in [" << data_root << "]" << std::endl;
        signs_dir.clear();
        return;
    }

    signs_dir.clear();
    std::vector<std::string>::iterator it = files.begin();
    while (it != files.end()) {
        SignId id = (SignId)atoi(it->c_str());
        signs_dir[id] = *it;
        ++it;
    }
}

const std::map<SignId, std::string> &SignDataLoader::GetSavedSignDir() const {
    return signs_dir;
}

SignDatas SignDataLoader::Load(const Glosses &glosses) {
    SignDatas sign_datas;
    Glosses::const_iterator it = glosses.begin();
    while (it != glosses.end()) {
        const LexicalItem &lexical_item = *it;
        SignDatas part_sign_datas = this->LoadPartSignDatas(lexical_item);
        for (SignData &sign_data : part_sign_datas) {
            if (sign_data.IsValid()) sign_datas.push_back(sign_data);
        }
        ++it;
    }
    return sign_datas;
}

SignDatas SignDataLoader::LoadPartSignDatas(const LexicalItem &lexical_item) {
    SignDatas sign_datas;
    switch (lexical_item.Type()) {
        case LexicalType::kWord:
            sign_datas.push_back(loadSignWord(lexical_item));
            break;
        case LexicalType::kPhrase:
            sign_datas = loadSignPhrase(lexical_item);
            break;
        case LexicalType::kClassifier:
            // load_sign_classifier(lexical_item);
            break;
        default: break;
    }
    return sign_datas;
}

SignData SignDataLoader::loadSignWord(const LexicalItem &lexical_item) {
    // Query the id
    const SignIds ids = this->SearchSignIds(lexical_item.GetWord());
    if (ids.size() > 0) {
        if (ids.size() > 1) {
            std::cout << "--> Warning: you are about to load a word but there's more than one sign id:\n    ";
            for (const SignId &id : ids) {
                std::cout << id << " ";
            }
            std::cout << std::endl;
            std::cout << "    only the first id will be used." << std::endl;
        }
        Params params;
        SignId id = ids[0];
        return SignData(id, LoadParams(id));
    }
    return SignData();
}

SignDatas SignDataLoader::loadSignPhrase(const LexicalItem &lexical_item) {
    // Query the id
    SignDatas sign_datas;
    const SignIds ids = this->SearchSignIds(lexical_item.GetWord());
    if (ids.size() > 0) {
        if (ids.size() <= 1) {
            std::cout << "--> Warning: you are about to load a phrase but there's only one sign id: " << ids[0] << std::endl;
        }
        for (const SignId &id : ids) {
            Params params = LoadParams(id);
            sign_datas.push_back(SignData(id, params));
        }
    }
    return sign_datas;
}

SignIds SignDataLoader::SearchSignIds(const std::string &word) {
    GlossesMap::iterator it = glosses_map.find(word);
    if (it != glosses_map.end()) {
        return it->second;
    }
    return SignIds();
}

Params SignDataLoader::LoadParams(const SignId &sign_id) {
    Params params;
    std::map<SignId, std::string>::iterator it = signs_dir.find(sign_id);
    if (it != signs_dir.end()) {
        std::string data_path = this->GetDataRoot() + it->second;
        try {
            csv::CSVReader<4> reader(data_path);
            reader.read_header(csv::ignore_extra_column, "frame", SIGNER_BODY_SAVED_HEADER, SIGNER_RHAND_SAVED_HEADER, SIGNER_LHAND_SAVED_HEADER);
            Frame frame;
            std::string mbody_theta, rhand_theta, lhand_theta;
            while (reader.read_row(frame, mbody_theta, rhand_theta, lhand_theta)) {
                FrameParams frame_params;
                std::vector<float> mbody_thetas = utils::ConvertToVectorFloat(mbody_theta);
                std::vector<float> rhand_thetas = utils::ConvertToVectorFloat(rhand_theta);
                std::vector<float> lhand_thetas = utils::ConvertToVectorFloat(lhand_theta);
                frame_params.insert(frame_params.end(), mbody_thetas.begin(), mbody_thetas.end());
                frame_params.insert(frame_params.end(), rhand_thetas.begin(), rhand_thetas.end());
                frame_params.insert(frame_params.end(), lhand_thetas.begin(), lhand_thetas.end());
                params.push_back(frame_params);
            }
        }
        catch (csv::error::can_not_open_file err) {
            std::cout << "!!! Error: Can't open file.\nDetail: " << err.what() << std::endl;
        }
        catch (...) {
            std::cout << "!!! Unknow Error while reading " << data_path << "." << std::endl;
        }
    }
    return params;
}
