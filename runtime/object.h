#ifndef __joyeer_runtime_object_h__
#define __joyeer_runtime_object_h__

#include <vector>

typedef int64_t     JrInt;
typedef int64_t     JrInt64;
typedef int32_t     JrInt32;
typedef int16_t     JrInt16;
typedef int8_t      JrInt8;

typedef uint64_t    JrUInt;
typedef uint64_t    JrUInt64;
typedef uint32_t    JrUInt32;
typedef uint16_t    JrUInt16;
typedef uint8_t     JrUInt8;

typedef JrInt*      JrPtr;
typedef JrInt       JrObjectRef;

enum JrTypeKind: JrUInt8 {
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
    
};
 
#endif
