//
// Created by Qing Xu on 2021/11/15.
//

#include "joyeer/vm/gc.h"

intptr_t GC::allocate(MemoryArea area, Class *klass) {
    if(area == MemoryArea::Permanent) {
        return allocateForStatic(klass);
    } else {
        return reinterpret_cast<intptr_t>(allocateForDynamic(klass));
    }
}

intptr_t GC::allocateForStatic(Class *klass) {
    size_t size = klass->getStaticSize();
    auto address = heap->allocate(size);
    return address;
}

Object* GC::allocateForDynamic(Class *klass) {
    assert(false);
    size_t size = klass->getStaticSize();
    auto address = heap->allocate(size);
    Object* object = (Object*)address;
    object->klass = klass;
    return object ;
}
