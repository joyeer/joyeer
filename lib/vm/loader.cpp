//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/loader.h"
#include "joyeer/vm/isolate.h"

void ClassLoader::load(const ModuleType::Ptr& module) {
    auto moduleClass = new ModuleClass();
    // register ModuleClass with ModuleType
    isolateVM->register_(module, moduleClass);

    auto variables = module->getVariables();
    for(auto const& variable: variables ) {
        auto field = compile(variable);
        moduleClass->staticFields.push_back(field);
    }

    isolateVM->import(moduleClass);
    // static memory allocation
    compile(module->instructions);
}


void ClassLoader::compile(const std::vector<Instruction> &instructions) {
    auto result = std::vector<Instruction>();
    for(auto const& instruction : instructions) {
        if(instruction.opcode == OP_GETSTATIC) {

        }
    }
}

Field ClassLoader::compile(const VariableType::Ptr& variableType) {
    auto type = compilerService->getType(variableType->addressOfType);
    switch (type->kind) {
        case ValueType::Int:
            return Field(ValueType::Int);
        default:
            assert(false);
    }
    assert(false);
}


