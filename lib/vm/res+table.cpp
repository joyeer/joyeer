//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/res+table.h"

void StringTable::import(std::vector<std::string>& strings) {
    this->strings = strings;
}

const std::string &StringTable::operator[](int index) {
    return strings[index];
}

