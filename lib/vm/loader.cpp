//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/loader.h"


void ClassLoader::compile(const ModuleType::Ptr& module, CompilerService* pService) {
    this->compilerService = pService;
    auto moduleClass = new ModuleClass();
    auto variables = module->getVariables();
    for(auto const& variable: variables ) {
        auto field = compile(variable);
        moduleClass->staticFields.push_back(field);
    }
    compile(module->instructions);
}

void ClassLoader::compile(const std::vector<Instruction> &instructions) {

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


