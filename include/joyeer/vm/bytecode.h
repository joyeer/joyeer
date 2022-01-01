//
// Created by Qing Xu on 2021/11/28.
//

#ifndef __joyeer_vm_bytecode_h__
#define __joyeer_vm_bytecode_h__

#include "joyeer/vm/memory.h"
#include "joyeer/runtime/bytecode.h"


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
