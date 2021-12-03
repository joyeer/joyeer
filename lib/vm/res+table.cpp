//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/res+table.h"
#include "joyeer/vm/stdlib.h"



void StringTable::import(std::vector<std::string>& strings) {
    this->strings = strings;
}

const std::string &StringTable::operator[](int index) {
    return strings[index];
}

void MethodResTable::import(CompilerService *compilerService) {

    for(auto const& type: compilerService->types) {
        if(type->kind == ValueType::Func) {
            if(type->address == static_cast<size_t>(BuildIns::Func_Print)) {
                auto method = new Global_$_print();
                method->slotID = methods.size();
                methods.push_back(method);
                mapOfTypeAndMethod[type->address] = method->slotID;
            } else {
                assert(false);
            }
        }
    }
}

Method* MethodResTable::query(const FuncType::Ptr& funcType) {
    return methods[mapOfTypeAndMethod[funcType->address]];
}
