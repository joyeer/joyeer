#ifndef __joyeer_runtime_object_h__
#define __joyeer_runtime_object_h__

#include <vector>



struct JrObjectHead {
    size_t  refCount;
    size_t  size;
    int     type;
};

struct JrObject {
    JrObjectHead head;
    void*        data;
};

struct JrPrimaryObject {
    JrObjectHead head;
    
};

struct JrObjectAlloc {
    
};

#endif
