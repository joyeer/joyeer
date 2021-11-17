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

struct PermanentArea {
    JrByte* memory = nullptr;
};

#endif //__joyeer_vm_memory_h__
