#ifndef __joyeer_runtime_object_h__
#define __joyeer_runtime_object_h__

#include <vector>

typedef int64_t     JrInt;
typedef uint8_t     JrByte;

typedef JrInt*      JrPtr;
typedef JrInt       JrObjectRef;

enum JrTypeKind: JrByte {
    typeVoid = 0x01,
    typeInt,
    typeFloat,
    typeDouble,
    typeLong,
    typeBoolean,
    typeString,
    typeAny,
    typeNil,
    typeObject
};

struct JrObjectHead {
    JrInt  refCount;
    JrInt  size;
};

struct JrObject {
    static const JrObject* nil;
    
    JrObjectHead head;
    
    void setField(JrInt objectRef, JrInt addressOfField);
    JrInt getFieldAsObjectRef(JrInt addressOfField);
};

struct JrPrimaryObject {
    JrObjectHead head;
};

struct JrValueHold {
    JrTypeKind kind;
    union {
        JrInt           intValue;
        JrObjectRef     objRefValue;
    };
};
 
#endif
