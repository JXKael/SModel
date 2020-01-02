#ifndef LEXICAL_ITEM_H
#define LEXICAL_ITEM_H

#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace signs {

enum LexicalType {
    kWord = 0,
    kPhrase,
    kClassifier
};

class LexicalItem {
private:
    std::string word_;
    LexicalType type_;

public:
    LexicalItem();
    explicit LexicalItem(const std::string &word, const LexicalType &type = LexicalType::kWord);
    ~LexicalItem();

    LexicalItem &operator=(const LexicalItem &) = delete;

    inline const std::string GetWord() const { return word_; }

    inline const LexicalType &Type() const { return type_; }
};

typedef std::vector<LexicalItem> Glosses;

} // namespace singer

#endif // LEXICAL_ITEM_H
