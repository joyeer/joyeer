//
// Created by Qing Xu on 2021/11/15.
//

#include "joyeer/vm/gc.h"

JrObject *GC::allocate(MemoryArea, JrClass *klass) {
    size_t size = klass->getSize();
    auto address = heap->allocate(size);
    return nullptr;
}
