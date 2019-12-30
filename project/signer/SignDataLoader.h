#ifndef SIGN_DATA_LOADER_H
#define SIGN_DATA_LOADER_H

#include "SignData.h"
#include "utils/csv.h"
#include "utils/dir_utils.h"

#include <iostream>
#include <fstream>
#include <string>

namespace signer {

class SignDataLoader {
private:
    std::string root_;

    std::map<SignId, std::string> signs_dir; // id -> file_name
    std::map<std::string, SignId> glosses_map; // gloss -> sign_id
public:
    SignDataLoader();
    explicit SignDataLoader(const std::string &root);

    ~SignDataLoader();

    void Init();
    const std::string GetDataRoot();
    const std::string GetGlossesMapFilePath();
    inline std::vector<std::string>::size_type Size() { return signs_dir.size(); }

    SignData Load(const LexicalItem &lexical_item);
    SignId SearchSignId(const LexicalItem &lexical_item);
};

} // namespace singer

#endif // SIGN_DATA_LOADER_H
