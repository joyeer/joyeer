//
// Created by Qing Xu on 2021/11/15.
//

#ifndef __joyeer_vm_gc_h__
#define __joyeer_vm_gc_h__

#include "heaps.h"

enum MemoryArea : uint8_t {
    Permanent,
    Dynamic
};

class GC {
public:
    intptr_t allocate(MemoryArea area, Type* type);
    intptr_t allocate(Class* klass, size_t size);
    static void write(intptr_t memory, Value value, int offset);
    static Value read(intptr_t memory, int offset);
private:
    intptr_t allocateForStatic(Class* klass);
private:
    Heap* heap { new Heap() };
};


#endif //__joyeer_vm_gc_h__
