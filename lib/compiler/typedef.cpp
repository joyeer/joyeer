//
// Created by Qing Xu on 2021/10/16.
//

#include "joyeer/compiler/typedef.h"

JrNilTypeDef::JrNilTypeDef():
        JrTypeDef("nil", JrTypeKind::Nil){}

JrVoidTypeDef::JrVoidTypeDef() :
        JrTypeDef("void", JrTypeKind::Void) {}

JrIntTypeDef::JrIntTypeDef() :
        JrTypeDef("Int", JrTypeKind::Integer) {}

JrBoolTypeDef::JrBoolTypeDef() :
        JrTypeDef("Bool", JrTypeKind::Integer) {}

JrBlockTypeDef::JrBlockTypeDef() :
        JrTypeDef("Block", JrTypeKind::Block),
        localVars() {}

JrFuncTypeDef::JrFuncTypeDef(const std::string &name) :
        JrTypeDef(name, JrTypeKind::Function),
        funcKind(JrFuncTypeKind::VM_Func),
        paramTypes(),
        returnType(nullptr) {}

JrClassTypeDef::JrClassTypeDef(const std::string &name) :
        JrTypeDef(name, JrTypeKind::Function) {}

JrFileModuleTypeDef::JrFileModuleTypeDef(const std::string &name) :
        JrClassTypeDef(name) {
    kind = JrTypeKind::FileModule;
}

