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

JrNilType::JrNilType():
        JrType("nil", JrTypeKind::Nil){}

JrVoidType::JrVoidType() :
        JrType("void", JrTypeKind::Void) {}

JrIntType::JrIntType() :
        JrType("Int", JrTypeKind::Integer) {}

JrBoolType::JrBoolType() :
        JrType("Bool", JrTypeKind::Integer) {}

JrBlockType::JrBlockType() :
        JrType("Block", JrTypeKind::Block),
        localVars() {}

JrFuncType::JrFuncType(const std::string &name) :
        JrType(name, JrTypeKind::Function),
        funcKind(JrFuncTypeKind::VM_Func),
        paramTypes(),
        returnType(nullptr) {}

JrClassType::JrClassType(const std::string &name) :
        JrType(name, JrTypeKind::Function) {}

JrFileModuleType::JrFileModuleType(const std::string &name) :
        JrClassType(name),
        instructions() {
    kind = JrTypeKind::FileModule;
}

