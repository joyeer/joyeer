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

#define JrType_Void         0
#define JrType_Int          1
#define JrType_Float        2
#define JrType_Double       3
#define JrType_Long         4
#define JrType_Boolean      5
#define JrType_String       7

#define JrType_Any          8
#define JrType_Nil          9
#define JrType_Object       10

struct JrObjectHead {
    JrInt  refCount;
    JrInt  size;
    JrInt  type;
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
 
#endif
