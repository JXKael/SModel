#ifndef LEXICAL_ITEM_H
#define LEXICAL_ITEM_H


#include <iostream>
#include <string>
#include <map>
#include <vector>

namespace signer {

enum LexicalType {
    kWord = 0,
    kClassifier
};

class LexicalItem {
private:
    std::string word_;
    LexicalType type_;

public:
    LexicalItem();
    LexicalItem(const LexicalItem &lexical_item);
    explicit LexicalItem(const std::string &word, const LexicalType &type=LexicalType::kWord);
    ~LexicalItem();

    inline const std::string GetWord() const { return word_; }
};

typedef std::vector<LexicalItem> Glosses;

} // namespace singer

#endif // LEXICAL_ITEM_H
