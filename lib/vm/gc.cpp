//
// Created by Qing Xu on 2021/11/15.
//

#include "joyeer/vm/gc.h"

Object *GC::allocate(MemoryArea, Class *klass) {
    size_t size = klass->getSize();
    auto address = heap->allocate(size);
    return nullptr;
}
