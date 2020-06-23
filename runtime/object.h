#ifndef __joyeer_runtime_object_h__
#define __joyeer_runtime_object_h__

#include <vector>

typedef int JrType;

#define JrType_Int          1
#define JrType_Float        2
#define JrType_Double       3
#define JrType_Long         4

#define JrType_String       8


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

#endif
