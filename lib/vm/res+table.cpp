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

void ClassResTable::import(const ClassType::Ptr &classType, Class *klass) {
    klass->slot = static_cast<int>(classes.size());
    classes.push_back(klass);
    mapOfTypeAndClass[classType->slot] = klass->slot;
}

Class* ClassResTable::query(const ClassType::Ptr &classType) {
    auto klassSlotID = mapOfTypeAndClass[classType->slot];
    return const_cast<Class *>(classes[klassSlotID]);
}

const Class *ClassResTable::operator[](Slot slot) {
    return classes[slot];
}

//////////////////////////////////////////
//  MethodResTable
//////////////////////////////////////////

Method* MethodResTable::query(const FuncType::Ptr& funcType) {
    if(mapOfTypeAndMethod.contains(funcType->slot)) {
        return methods[mapOfTypeAndMethod[funcType->slot]];
    }
    return nullptr;
}

void MethodResTable::import(Method *method, const Type::Ptr& funcType) {
    assert(method->slot == -1);
    method->slot = static_cast<int>(methods.size());
    methods.push_back(method);
    mapOfTypeAndMethod[funcType->slot] = method->slot;
}

const Method* MethodResTable::operator[](Slot slot) {
    return methods[slot];
}
