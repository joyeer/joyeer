//
// Created by Qing Xu on 2021/11/15.
//

#include "joyeer/vm/gc.h"

intptr_t GC::allocate(MemoryArea area, Type *klass) {
    if(area == MemoryArea::Permanent) {
//        return allocateForStatic(klass);
    } else {
//        return reinterpret_cast<intptr_t>(allocateForDynamic(klass));
    }

    assert(false);
}

intptr_t GC::allocate(Class *klass, size_t size) {
    auto address = heap->allocate(size + kIntSize);
    Object* object = (Object*)address;

    return reinterpret_cast<intptr_t>(object);
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
//    object->klass = klass;
    return object ;
}

void GC::write(intptr_t memory, Value value, int offset) {
    Value *address = reinterpret_cast<Value*>(memory) + offset;
    *address = value;
}

Value GC::read(intptr_t memory, int offset) {
    return *(reinterpret_cast<Value*>(memory) + offset);
}

