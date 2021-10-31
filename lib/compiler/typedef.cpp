//
// Created by Qing Xu on 2021/10/16.
//

#include "joyeer/compiler/typedef.h"
#include <sstream>

std::string debugAccessFlag(JrAccessFlag flag) {
    bool one = false;
    std::stringstream  ss;

#define ACCESS_FLAGS_PRINT(name) \
    if(one) { \
        ss << " "; \
    } \
    ss << (name); \
    one = true;

    if((flag & JrAccessFlag::Public) == JrAccessFlag::Public) {
        ACCESS_FLAGS_PRINT("public")
    }

    if((flag & JrAccessFlag::Protected) == JrAccessFlag::Protected) {
        ACCESS_FLAGS_PRINT("protected")
    }

    if((flag & JrAccessFlag::Private) == JrAccessFlag::Private) {
        ACCESS_FLAGS_PRINT("private")
    }

    if((flag & JrAccessFlag::Static) == JrAccessFlag::Static) {
        ACCESS_FLAGS_PRINT("static")
    }

    return ss.str();
}

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

