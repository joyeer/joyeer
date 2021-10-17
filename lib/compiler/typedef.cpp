//
// Created by Qing Xu on 2021/10/16.
//

#include "joyeer/compiler/typedef.h"

JrNilTypeDef::JrNilTypeDef():
        JrTypeDef("nil", JrTypeType::Nil){}

JrVoidTypeDef::JrVoidTypeDef() :
        JrTypeDef("void", JrTypeType::Void) {}

JrFuncTypeDef::JrFuncTypeDef(const std::string &name) :
        JrTypeDef(name, JrTypeType::Function) {}

JrClassTypeDef::JrClassTypeDef(const std::string &name) :
        JrTypeDef(name, JrTypeType::Function) {}

JrModuleTypeDef::JrModuleTypeDef(const std::string &name) :
        JrClassTypeDef(name) {
    type = JrTypeType::Module;
}

JrIntTypeDef::JrIntTypeDef() :
        JrTypeDef("Int", JrTypeType::Integer) {}

JrBoolTypeDef::JrBoolTypeDef() :
        JrTypeDef("Bool", JrTypeType::Integer) {}

