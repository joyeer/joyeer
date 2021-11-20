//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/compiler.h"


void VCompiler::compile(JrFileModuleType::Ptr fileModule) {
    compile(fileModule->instructions);
}

void VCompiler::compile(const std::vector<Instruction> &instructions) {
}

