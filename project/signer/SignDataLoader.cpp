#include "SignDataLoader.h"

using namespace signer;

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
        reader.read_header(csv::ignore_extra_column, "id", "gloss");
        unsigned int id;
        std::string gloss;
        while (reader.read_row(id, gloss)) {
            std::vector<std::string> glosses = utils::ConvertToVectorString(gloss);
            for (std::string word : glosses) {
                glosses_map[word] = id;
            }
        }
    }
    catch (csv::error::can_not_open_file err) {
        std::cout << "!!! Error: Can't open file.\nDetail: " << err.what() << std::endl;
    }
    catch (...) {
        std::cout << "!!! Unknow Error while reading " << map_path << "." << std::endl;
    }

    // data
    std::string data_root = GetDataRoot();

    std::vector<std::string> files = utils::listFiles(data_root.c_str());
    if (files.size() <= 0) {
        std::cout << "--> Warning: nothing in [" << data_root << "]" << std::endl;
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

const std::string SignDataLoader::GetDataRoot() {
    return root_ + "/sign_data";
}

const std::string SignDataLoader::GetGlossesMapFilePath() {
    return root_ + "/glosses_map/glosses_map.csv";
}

SignData SignDataLoader::Load(const LexicalItem &lexical_item) {
    // Query the id
    SignId id = this->SearchSignId(lexical_item);
    SignData sign_data(id, lexical_item);
    std::map<SignId, std::string>::iterator it = signs_dir.find(id);
    if (it != signs_dir.end()) {
        sign_data.LoadParams(GetDataRoot());
    }
    return sign_data;
}

SignId SignDataLoader::SearchSignId(const LexicalItem &lexical_item) {
    const std::string word = lexical_item.GetWord();
    std::map<std::string, SignId>::iterator it = glosses_map.find(word);
    if (it != glosses_map.end()) {
        return it->second;
    }
    return -1;
}
