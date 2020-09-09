#include "object.h"
#include "runtime.h"

const JrObject* JrObject::Nil = new JrObject();
const JrObjectManaged* JrObjectManaged::Nil = new JrObjectManaged();

#define JrObject_Field(adddressOfField) \
    *(JrValueHold*)((JrByte*)this + sizeof(JrObjectHead) + addressOfField * sizeof(JrValueHold))

void JrObject::setField(JrValueHold value, JrInt addressOfField) {
    JrObject_Field(addressOfField) = value;
}

JrValueHold JrObject::getFieldAsObjectRef(JrInt addressOfField) {
    return JrObject_Field(addressOfField);
}


auto JrObjectManagedInitializer = [](JrPtr memory) -> JrObject* {
    return new(memory) JrObjectManaged;
};

auto JrObjectMangedFinalizer = [](JrObject* object) {
    auto arrayObject = (JrObjectManaged*)object;
    arrayObject->~JrObjectManaged();
};

struct JrObjectManagedType : public JrObjectType {
    JrObjectManagedType():
        JrObjectType(L"Object", JrObjectManagedInitializer, JrObjectMangedFinalizer) {
    }
    
    JrInt size() {
        return sizeof(JrObjectManaged);
    }
};

JrObjectManaged::JrObjectManaged() {
    
}

JrObjectManaged::~JrObjectManaged() {
}

