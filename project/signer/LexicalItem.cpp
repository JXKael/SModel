#include "LexicalItem.h"

using namespace signer;

LexicalItem::LexicalItem() {

}

LexicalItem::LexicalItem(const LexicalItem &lexical_item) {
    this->word_ = lexical_item.word_;
    this->type_ = lexical_item.type_;
}

LexicalItem::LexicalItem(const std::string &word, const LexicalType &type)
  : word_(word),
    type_(type)
{

}

LexicalItem::~LexicalItem() {

}
