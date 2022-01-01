//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/runtime/bytecode.h"
#include "joyeer/vm/loader.h"
#include "joyeer/vm/types.h"

ModuleClass* ClassLoader::load(const ModuleType::Ptr& module) {
    auto moduleClass = new ModuleClass();
    // register ModuleClass with ModuleType
    isolateVM->classes->import((const ClassType::Ptr &) module, moduleClass);

    auto variables = module->getVariables();
    for(auto const& variable: variables ) {
        auto field = compile(variable);
        moduleClass->staticFields.push_back(field);
    }

    isolateVM->import(moduleClass);
    // static memory allocation
    auto moduleInitializer = compile(module);

    moduleClass->initializerSlot = moduleInitializer->slot;
    return moduleClass;
}

Method *ClassLoader::compile(const ModuleType::Ptr& module) {
    auto method = new VMethod(module->bytecodes, 0, module->getVariables().size());
    isolateVM->methods->import(method, module);
    return method;
}

Method* ClassLoader::compile(const FuncType::Ptr &funcType){

    auto paramCount = funcType->paramTypes.size();
    auto localVarCount = paramCount + funcType->getLocalVarCount();
    auto method = new VMethod(funcType->bytecodes, paramCount, localVarCount);

    isolateVM->methods->import(method, funcType);
    return method;
}

Field ClassLoader::compile(const Variable::Ptr& variable) {
    auto type = compilerService->getType(variable->typeSlot);
    switch (type->kind) {
        case ValueType::Int:
            return Field(ValueType::Int);
        case ValueType::Bool:
            return Field(ValueType::Bool);
        case ValueType::Class:
            return Field(ValueType::Class);
        case ValueType::Any:
            return Field(ValueType::Any);
        default:
            assert(false);
    }
    assert(false);
}



