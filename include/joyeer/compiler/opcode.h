//
// Created by Qing Xu on 2021/10/19.
//

#ifndef __joyeer_compiler_opcode_h__
#define __joyeer_compiler_opcode_h__

#include <vector>
#include <string>
#include "joyeer/runtime/bytecode.h"

//
struct Instruction {
    Opcode opcode;
    union {
        int64_t value = -1;
        struct {
            int32_t val1 = -1;
            int32_t val2 = -1;
        } pair;
    };

};

struct JrCodeWriter {

    void write(const Instruction& instruction);
    void write(const std::vector<Instruction>& instructions);

    std::vector<Instruction> instructions;
};

// debug output the instruction
std::string debugPrint(const Instruction& instruction);


#endif //__joyeer_compiler_opcode_h__
