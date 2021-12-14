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

void ClassResTable::register_(const ModuleType::Ptr &moduleType, Class *klass) {
    klass->slot = static_cast<int>(classes.size());
    classes.push_back(klass);
    mapOfTypeAndClass[moduleType->address] = klass->slot;
}

Class* ClassResTable::query(const ClassType::Ptr &classType) {
    auto klassSlotID = mapOfTypeAndClass[classType->address];
    return const_cast<Class *>(classes[klassSlotID]);
}

const Class *ClassResTable::operator[](Slot slot) {
    return classes[slot];
}

void MethodResTable::import(CompilerService *compilerService) {

    for(auto const& type: compilerService->types) {
        if(type->kind == ValueType::Func) {
            if(type->address == static_cast<size_t>(BuildIns::Func_Print)) {
                auto method = new Global_$_print();
                import(method);
                mapOfTypeAndMethod[type->address] = method->slot;
            } else {
                assert(false);
            }
        }
    }
}

Method* MethodResTable::query(const FuncType::Ptr& funcType) {
    return methods[mapOfTypeAndMethod[funcType->address]];
}

void MethodResTable::import(Method *method) {
    assert(method->slot == -1);
    method->slot = static_cast<int>(methods.size());
    methods.push_back(method);
}

const Method* MethodResTable::operator[](Slot slot) {
    return methods[slot];
}
