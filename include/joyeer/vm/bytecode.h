//
// Created by Qing Xu on 2021/11/28.
//

#ifndef __joyeer_vm_bytecode_h__
#define __joyeer_vm_bytecode_h__

#include "joyeer/vm/memory.h"

/**
 * | - 8 -  | -- 56 -- |
 * | opcode |   value  |
 */

struct Bytecode {
    union {
        struct {
            int8_t op: 8 = -1;
            int64_t value: 56 = -1;
        } format1 ;
        struct {
            int8_t op: 8 =-1;
            int32_t value1: 24 = -1;
            int32_t value2: 32 = -1;
        } format2;
    };

    Bytecode(int8_t op, int64_t value) {
        format1.op = op;
        format1.value = value;
    }

    Bytecode(int8_t op, int32_t value1, int32_t value2) {
        format2.op = op;
        format2.value1 = value1;
        format2.value2 = value2;
    }
};


#define DEF_BYTECODE(op, value) \
    Bytecode((int8_t)(op), (int64_t)(value))

#define DEF_BYTECODE_2(op, value1, value2) \
    Bytecode((int8_t)(op), (int32_t)(value1), (int32_t)(value2))

#define OP_FROM_BYTECODE(bytecode) (int8_t)((bytecode).format1.op)

#define VALUE_FROM_BYTECODE(bytecode) (int64_t)((bytecode).format1.value)

#define VAL1_FROM_BYTECODE(bytecode) (int32_t)((bytecode).format2.value1)
#define VAL2_FROM_BYTECODE(bytecode) (int32_t)((bytecode).format2.value2)

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
