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
constexpr int kBytecodeValue2Offset = 32;

#define DEF_BYTECODE(op, value) \
    (int64_t)((int32_t)(op) | (int32_t)(value) << kBytecodeOpWidth)

#define DEF_BYTECODE_2(op, value1, value2) \
    (int64_t)((int64_t)(op) | (int64_t)(value2) << kBytecodeValue2Offset | (int64_t)(value1) << kBytecodeOpWidth)

#define OP_FROM_BYTECODE(bytecode) (BytecodeOp)((bytecode) & 0x000000FF)

#define VALUE_FROM_BYTECODE(bytecode) (Int)(((bytecode) & 0xFFFFFFFFFFFFFF00) >> kBytecodeOpWidth)

#define VAL1_FROM_BYTECODE(bytecode) (Int)(((bytecode) & 0xFFFFFF00) >> kBytecodeOpWidth)
#define VAL2_FROM_BYTECODE(bytecode) (Int)(((bytecode) & 0xFFFFFFFF00000000) >> kBytecodeValue2Offset)

struct Bytecodes {
    Bytecodes(Byte* bytes, int length);
    ~Bytecodes();

    Byte *bytecodes;
    Int size{};
};

class BytecodeWriter {
public:
    void write(Bytecode bytecode);
    [[nodiscard]] Bytecodes* getBytecodes() const;
private:
    MemoryBuffer buffer {};
};


#endif //__joyeer_vm_bytecode_h__
