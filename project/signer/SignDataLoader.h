#ifndef SIGN_DATA_LOADER_H
#define SIGN_DATA_LOADER_H

#include "LexicalItem.h"
#include "SignData.h"
#include "utils/csv.h"
#include "utils/dir_utils.h"

#include <iostream>
#include <fstream>
#include <string>

#define SIGNER_BODY_SAVED_HEADER "mbody_theta"
#define SIGNER_RHAND_SAVED_HEADER "rhand_theta"
#define SIGNER_LHAND_SAVED_HEADER "lhand_theta"

namespace signs {

// gloss -> sign_ids
typedef std::map<std::string, SignIds> GlossesMap;

class SignDataLoader {
private:
    std::string root_;

    std::map<SignId, std::string> signs_dir; // id -> file_name
    GlossesMap glosses_map;
public:
    SignDataLoader();
    explicit SignDataLoader(const std::string &root);

    ~SignDataLoader();

    void Init();
    const std::string GetDataRoot();
    const std::string GetGlossesMapFilePath();
    inline std::vector<std::string>::size_type Size() { return signs_dir.size(); }

    SignDatas Load(const Glosses &glosses);
    SignDatas SignDataLoader::LoadPartSignDatas(const LexicalItem &lexical_item);
    SignIds SearchSignIds(const std::string &word);
    Params LoadParams(const SignId &sign_id);

    void ReloadSignsDir();
    const std::map<SignId, std::string> &GetSavedSignDir() const;

private:
    SignData loadSignWord(const LexicalItem &lexical_item);
    SignDatas loadSignPhrase(const LexicalItem &lexical_item);
};

} // namespace singer

#endif // SIGN_DATA_LOADER_H
