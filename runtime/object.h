#ifndef __joyeer_runtime_object_h__
#define __joyeer_runtime_object_h__

#include <vector>

typedef int64_t     JrInt;
typedef int32_t     JrInt32;

typedef uint8_t     JrByte;

typedef JrInt*      JrPtr;
typedef JrInt       JrObjectRef;

struct JrValueHold;
struct JrObjectType;

enum JrTypeKind: JrByte {
    typeVoid            = 0x01,
    typeInt             = 0x02,
    typeFloat           = 0x03,
    typeDouble          = 0x04,
    typeLong            = 0x05,
    typeBoolean         = 0x06,
    typeString          = 0x07,
    typeAny             = 0x08,
    typeNil             = 0x09,
    typeObject          = 0x0A
};

struct JrObjectHead {
    JrInt32  refCount;
    JrInt32  size;
};

struct JrObject {
    static const JrObject* Nil;
    
    JrObjectHead head;
};

struct JrObjectManaged: public JrObject {
    
    JrObjectManaged(JrObjectType* type);
    ~JrObjectManaged();
    
    JrObjectType* type;
    std::vector<JrValueHold> fields;
    
    void setField(JrInt addressOfField, const JrValueHold &value);
    JrValueHold getField(JrInt addressOfField);
};

struct JrValueHold {
    JrTypeKind kind;
    union {
        JrInt           intValue;
        JrObjectRef     objRefValue;
    };
    
    bool operator== (const JrValueHold& value) const {
        return value.kind == kind && value.intValue == intValue;
    }
};
 
#endif
