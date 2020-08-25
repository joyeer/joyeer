#include "object.h"

const JrObject* JrObject::nil = new JrObject();

#define JrObject_Field(adddressOfField) \
    *(JrValueHold*)((JrByte*)this + sizeof(JrObjectHead) + addressOfField * sizeof(JrValueHold))

void JrObject::setField(JrValueHold value, JrInt addressOfField) {
    JrObject_Field(addressOfField) = value;
}

JrValueHold JrObject::getFieldAsObjectRef(JrInt addressOfField) {
    return JrObject_Field(addressOfField);
}
