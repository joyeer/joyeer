#ifndef __joyeer_runtime_gc_h__
#define __joyeer_runtime_gc_h__

#include "object.h"
#include <stack>
#include "runtime.h"

struct JrObjectTable {
    std::vector<struct JrObject *> list;
    std::stack<int> freeSlots;
    
    int registerObject(JrObject* object);
};

struct JrObjectAlloc {
    JrObject* alloc(JrObjectType::Pointer type);
};

struct JrGC {
    JrObjectAlloc *allocator;
    JrObjectTable *objTable;
    
    JrGC();
    ~JrGC();
    
    int alloc(JrObjectType::Pointer type);
    
    JrObject* get(int objectRef);
};
#endif
