//
// Created by Qing Xu on 2021/11/13.
//

#ifndef __joyeer_vm_memory_h__
#define __joyeer_vm_memory_h__


#include "joyeer/vm/types.h"

class MemoryBuffer {
public:
    explicit MemoryBuffer(size_t initial = 1024) {
        resize(initial);
    }

    ~MemoryBuffer() {
        std::free(buffer);
    }

    void write(Byte byte) {
        tryResizeIfNeed(pos + kByteSize);
        *(Byte*)(buffer + pos) = byte;
        pos += kBoolSize;
    }

    void write(Int value) {
        tryResizeIfNeed(pos + kIntSize);
        *(Int*)(buffer + pos) = value;
        pos += kIntSize;
    }

protected:
    friend class BytecodeWriter;

    // try to resize memory buffer if the pos > capacity
    void tryResizeIfNeed(size_t newPos) {
        if(newPos > capacity) {
            resize(capacity * 2);
        }
    }

    void resize(size_t newSize);

    // raw data
    Byte * buffer = nullptr;
    size_t pos = 0;
    size_t capacity = 0;
};


#endif //__joyeer_vm_memory_h__
