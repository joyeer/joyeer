//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/compiler.h"



void VCompiler::compile(const ModuleType::Ptr& module, CompilerService* compilerService) {
    this->compilerService = compilerService;
    auto moduleClass = new ModuleClass();
    auto variables = module->getVariables();
    for(auto const& variable: variables ) {
        auto field = compile(variable);
        moduleClass->staticFields.push_back(field);
    }
    compile(module->instructions);
}

void VCompiler::compile(const std::vector<Instruction> &instructions) {

}

Field VCompiler::compile(const VariableType::Ptr& variableType) {
    switch (variableType->kind) {
        case TypeKind::Integer:
            return Field(ValueType::Int);
        default:
            assert(false);
    }
    assert(false);
}


