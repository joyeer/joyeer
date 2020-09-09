#ifndef __joyeer_runtime_object_h__
#define __joyeer_runtime_object_h__

#include <vector>

typedef int64_t     JrInt;
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
    JrInt  refCount;
    JrInt  size;
};

struct JrObject {
    static const JrObject* Nil;
    
    JrObjectHead head;
    
    void setField(JrValueHold value, JrInt addressOfField);
    JrValueHold getFieldAsObjectRef(JrInt addressOfField);
};

struct JrObjectManaged: public JrObject {
    static JrObjectType* Type;
    static const JrObjectManaged* Nil;
    
    JrObjectManaged();
    ~JrObjectManaged();
    
    std::vector<JrValueHold> fields;
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
