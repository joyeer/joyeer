#ifndef __joyeer_runtime_instruction_h__
#define __joyeer_runtime_instruction_h__

#include <stdint.h>
#include <cstddef>
#include <vector>

enum Opcode {
    OP_NONE = 0x00,
    OP_ICONST,
    OP_ICONST_64,
    
    OP_LOAD,
    OP_INVOKE,
    OP_DEBUG,

};

struct Instruction {
    uint8_t opcode;
    int32_t value;
};

class JrCodeWriter {
public:
    void write(const Instruction& instruction);
private:
    std::vector<Instruction> instructions;
};

#endif
