//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/compiler.h"


void VCompiler::compile(const ModuleType::Ptr& module) {
    auto moduleClass = new ModuleClass();
    auto variables = module->getVariables();
    compile(module->instructions);
}

void VCompiler::compile(const std::vector<Instruction> &instructions) {
}

