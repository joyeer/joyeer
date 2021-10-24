//
// Created by Qing Xu on 2021/10/16.
//

#include "joyeer/compiler/typedef.h"

JrNilTypeDef::JrNilTypeDef():
        JrTypeDef("nil", JrTypeType::Nil){}

JrVoidTypeDef::JrVoidTypeDef() :
        JrTypeDef("void", JrTypeType::Void) {}

JrIntTypeDef::JrIntTypeDef() :
        JrTypeDef("Int", JrTypeType::Integer) {}

JrBoolTypeDef::JrBoolTypeDef() :
        JrTypeDef("Bool", JrTypeType::Integer) {}

JrBlockTypeDef::JrBlockTypeDef() :
        JrTypeDef("Block", JrTypeType::Block),
        localVars() {}

JrFuncTypeDef::JrFuncTypeDef(const std::string &name) :
        JrTypeDef(name, JrTypeType::Function),
        funcType(JrFuncTypeType::VM_Func),
        paramTypes(),
        returnType(nullptr) {}

JrClassTypeDef::JrClassTypeDef(const std::string &name) :
        JrTypeDef(name, JrTypeType::Function) {}

JrFileModuleTypeDef::JrFileModuleTypeDef(const std::string &name) :
        JrClassTypeDef(name) {
    type = JrTypeType::Module;
}

