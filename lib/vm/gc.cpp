//
// Created by Qing Xu on 2021/11/15.
//

#include "joyeer/vm/gc.h"

Object *GC::allocate(MemoryArea, Class *klass) {
    size_t size = klass->getStaticSize();
    auto address = heap->allocate(size);
    Object* object = (Object*)address;
    object->klass = klass;
    return object   ;
}
