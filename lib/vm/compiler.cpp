//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/compiler.h"


void VCompiler::compile(FileModuleType::Ptr fileModule) {
    auto fileModuleClass = new JrFileModuleClass();
    compile(fileModule->instructions);
}

void VCompiler::compile(const std::vector<Instruction> &instructions) {
}

