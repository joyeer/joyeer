//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_compiler_h__
#define __joyeer_vm_compiler_h__

#include "joyeer/compiler/context.h"

class VCompiler {
public:
    void compile(const std::vector<Instruction>& instructions);
};
#endif //__joyeer_vm_compiler_h__
