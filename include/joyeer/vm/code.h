//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_code_h__
#define __joyeer_vm_code_h__

#include "joyeer/compiler/opcode.h"

class Code {
    Code(): instructions() {
    }

    std::vector<Instruction> instructions;
};

#endif //__joyeer_vm_code_h__
