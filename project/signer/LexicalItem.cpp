#include "LexicalItem.h"

using namespace signs;

LexicalItem::LexicalItem() {

}

LexicalItem::LexicalItem(const std::string &word, const LexicalType &type)
  : word_(word),
    type_(type)
{

}

LexicalItem::~LexicalItem() {

}
