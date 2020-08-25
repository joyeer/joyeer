#include "object.h"

const JrObject* JrObject::nil = new JrObject();

#define JrObject_Field(adddressOfField) \
    *(JrInt*)((JrByte*)this + sizeof(JrObjectHead) + addressOfField * sizeof(JrInt))

void JrObject::setField(JrInt objectRef, JrInt addressOfField) {
    JrObject_Field(addressOfField) = objectRef;
}

JrInt JrObject::getFieldAsObjectRef(JrInt addressOfField) {
    return JrObject_Field(addressOfField);
}
