#ifndef __joyeer_runtime_instruction_h__
#define __joyeer_runtime_instruction_h__

#include <stdint.h>
#include <cstddef>
#include <vector>

enum Opcode {
    OP_NOP = 0x00,
    OP_ICONST,
    OP_ICONST_64,
    OP_SCONST,
    
    OP_ISTORE,
    OP_ILOAD,
    OP_INVOKE,
    
    OP_DEBUG,

};

struct Instruction {
    uint8_t opcode;
    int32_t value;
};

struct JrCodeWriter {
    
    void write(const Instruction& instruction);
    
    std::vector<Instruction> instructions;
};

struct JrInstructionDebugPrinter {
    
};

#endif
