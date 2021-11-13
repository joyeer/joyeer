//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_compiler_h__
#define __joyeer_vm_compiler_h__

#include "joyeer/compiler/context.h"
#include "joyeer/vm/types.h"

class MemoryBuffer {
public:
    explicit MemoryBuffer(size_t initial = 1024) {
        resize(initial);
    }

    ~MemoryBuffer() {
        std::free(buffer);
    }

    void write(JrByte byte) {
        tryResizeIfNeed(pos + kJrByteSize);
        *(JrByte*)(buffer + pos) = byte;
        pos += kJrBoolSize;
    }

    void write(JrInt value) {
        tryResizeIfNeed(pos + kJrIntSize);
        *(JrInt*)(buffer + pos) = value;
        pos += kJrIntSize;
    }

protected:

    // try to resize memory buffer if the pos > capacity
    void tryResizeIfNeed(size_t newPos) {
        if(newPos > capacity) {
            resize(capacity * 2);
        }
    }

    void resize(size_t newSize) {
        if(newSize == 0) { // this check is not strictly needed,
            std::free(buffer);  // but zero-size realloc is deprecated in C
            buffer = nullptr;
        } else {
            if(void* mem = std::realloc(buffer, newSize))
                buffer = (JrByte *)mem;
            else
                throw std::bad_alloc(); //TODO: runtime exception??
        }

        capacity = newSize;
    }

    // raw data
    JrByte * buffer = nullptr;
    size_t pos = 0;
    size_t capacity = 0;
};


// VM compiler, compiling the Joyeer Compiler's file into interpreter accepted binary code

class VCompiler {
public:
    void compile(JrFileModuleTypeDef::Ptr fileModule);
    void compile(const std::vector<Instruction>& instructions);
private:
};

#endif //__joyeer_vm_compiler_h__
