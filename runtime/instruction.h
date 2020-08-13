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
    
    OP_ICMP,
    OP_IADD,
    OP_ISUB,
    OP_IMUL,
    OP_IDIV,
    OP_IREM,
    
    OP_IFEQ,    // == 0
    OP_IFNE,    // != 0
    OP_IFLT,    // < 0
    OP_IFLE,    // <= 0
    OP_IFGT,    // > 0
    OP_IFGE,    // >= 0
    
    OP_RETURN,
    OP_IRETURN,
    
    OP_ONEWARRAY,
    
    OP_INVOKE,
    
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
    const static std::unordered_map<Opcode, std::wstring> maps;
    std::wstring print(const Instruction& instruction);
};

#endif
                    
