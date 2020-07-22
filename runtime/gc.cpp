#include "gc.h"


JrObject* JrObjectAlloc::alloc(JrObjectType::Pointer type) {
    size_t headSize = sizeof(JrObjectHead);
    size_t size = headSize;
    for(auto field: type->virtualFields) {
        size += field->type->size;
    }
    auto objectData = (void*)malloc(size);
    auto object = (JrObject*)objectData;
    object->data = (void*)((size_t)objectData + headSize);
    return object;
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

int JrGC::alloc(JrObjectType::Pointer type) {
    auto object = allocator->alloc(type);
    return objTable->registerObject(object);
}

JrObject* JrGC::get(int objectRef) {
    return objTable->list[objectRef];
}
