//
// Created by Qing Xu on 2021/11/28.
//

#ifndef __joyeer_vm_bytecode_h__
#define __joyeer_vm_bytecode_h__

#include "joyeer/vm/memory.h"

/**
 * | - 8 -  | -- 65 -- |
 * | opcode |   value  |
 */
typedef int64_t Bytecode;
typedef uint8_t BytecodeOp;

constexpr int kBytecodeOpWidth = 8;

#define DEF_BYTECODE(op, value) (int64_t)((int64_t)op | (int64_t)value << kBytecodeOpWidth)

struct Bytecodes {
    Byte *bytecodes;
    Int size;
};

class BytecodeWriter {
public:
    void write(Bytecode bytecode);

private:
    MemoryBuffer buffer {};
};


#endif //__joyeer_vm_bytecode_h__
