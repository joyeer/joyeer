//
// Created by Qing Xu on 2022/1/2.
//

#include "joyeer/runtime/res+table.h"

int StringResTable::import(const std::string &string) {
    strings.push_back(string);
    return strings.size() - 1;
}

const std::string &StringResTable::operator[](int index) {
    return strings[index];
}

Type* TypeResTable::operator[](int index) {
    return types[index];
}

int TypeResTable::import(Type *type) {
    assert(type->slot == -1);
    types.push_back(type);
    type->slot = types.size() - 1;
    return type->slot;
}
