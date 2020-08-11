#include "gc.h"

JrObject* JrObjectAlloc::alloc(JrObjectType* type) {
    size_t size = type->size();
    auto object = (JrPtr)malloc(size);
    if(type->initializer != nullptr) {
        return type->initializer(object);
    } else {
        return (JrObject*)type;
    }
}

int JrObjectTable::registerObject(JrObject *object) {
    list.push_back(object);
    return list.size() - 1;
}

JrGC::JrGC():
allocator(new JrObjectAlloc()),
objTable(new JrObjectTable()){
    
}

JrGC::~JrGC() {
    delete allocator;
    delete objTable;
}

int JrGC::alloc(JrObjectType* type) {
    auto object = allocator->alloc(type);
    return objTable->registerObject(object);
}

JrObject* JrGC::get(int objectRef) {
    return objTable->list[objectRef];
}
