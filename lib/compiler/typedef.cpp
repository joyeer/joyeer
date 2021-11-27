//
// Created by Qing Xu on 2021/10/16.
//

#include "joyeer/compiler/typedef.h"
#include <sstream>

std::string debugAccessFlag(AccessFlag flag) {
    bool one = false;
    std::stringstream  ss;

#define ACCESS_FLAGS_PRINT(name) \
    if(one) { \
        ss << " "; \
    } \
    ss << (name); \
    one = true;

    if((flag & AccessFlag::Public) == AccessFlag::Public) {
        ACCESS_FLAGS_PRINT("public")
    }

    if((flag & AccessFlag::Protected) == AccessFlag::Protected) {
        ACCESS_FLAGS_PRINT("protected")
    }

    if((flag & AccessFlag::Private) == AccessFlag::Private) {
        ACCESS_FLAGS_PRINT("private")
    }

    if((flag & AccessFlag::Static) == AccessFlag::Static) {
        ACCESS_FLAGS_PRINT("static")
    }

    return ss.str();
}

NilType::NilType():
        Type("nil", ValueType::Nil){}

VoidType::VoidType() :
        Type("void", ValueType::Void) {}

IntType::IntType() :
        Type("Int", ValueType::Int) {}

BoolType::BoolType() :
        Type("Bool", ValueType::Bool) {}

BlockType::BlockType() :
        Type("Block", ValueType::Block),
        localVars() {}

FuncType::FuncType(const std::string &name) :
        Type(name, ValueType::Func),
        funcKind(FuncTypeKind::VM_Func),
        paramTypes(),
        returnType(nullptr) {}

ClassType::ClassType(const std::string &name) :
        Type(name, ValueType::Func) {}

ModuleType::ModuleType(const std::string &name) :
        ClassType(name),
        instructions() {
    kind = ValueType::Module;
}


