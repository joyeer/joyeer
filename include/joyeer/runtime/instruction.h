#ifndef __joyeer_runtime_instruction_h__
#define __joyeer_runtime_instruction_h__

#include "object.h"
#include <vector>
#include <unordered_map>

enum Opcode {
    OP_NOP = 0x00,
    
    OP_OCONST_NIL,
    OP_ICONST,      
    OP_ICONST_64,
    OP_SCONST,
    
    OP_ISTORE,
    
    OP_OLOAD,
    OP_ILOAD,
    
    OP_NEW,
    OP_PUTFIELD,
    OP_GETFIELD,
    
    OP_IAND,
    OP_ICMP_G,
    OP_ICMP_GE,
    OP_ICMP_L,
    OP_ICMP_LE,
    OP_ICMP_NE,
    OP_ICMP_EQ,
    OP_IADD,
    OP_ISUB,
    OP_IMUL,
    OP_IDIV,
    OP_IREM,
    OP_INEG,
    
    OP_IFEQ,    // == 0
    OP_IFNE,    // != 0
    OP_IFLT,    // < 0
    OP_IFLE,    // <= 0
    OP_IFGT,    // > 0
    OP_IFGE,    // >= 0
    
    OP_RETURN,
    OP_IRETURN,
    OP_ORETURN,
    
    OP_ONEWARRAY,
    
    OP_INVOKE,
    OP_DUP,
    OP_GOTO,
    
    OP_DEBUG,

};

struct Instruction {
    Opcode opcode;
    JrInt value = -1;
};

struct JrCodeWriter {
    
    void write(const Instruction& instruction);
    void write(const std::vector<Instruction>& instructions);
    
    std::vector<Instruction> instructions;
};

struct JrInstructionDebugPrinter {
    const static std::unordered_map<Opcode, std::string> maps;
    std::string print(const Instruction& instruction);
};

#endif
                    
