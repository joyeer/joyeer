#include "object.h"
#include "runtime.h"

const JrObject* JrObject::Nil = new JrObject();


JrObjectManaged::JrObjectManaged(JrObjectType* type):
type(type),
fields(type->virtualFields.size() + type->staticFields.size()) {
}

JrObjectManaged::~JrObjectManaged() {
}

void JrObjectManaged::setField(JrInt addressOfField, const JrValueHold &value) {
    assert(fields.size() > addressOfField);
    fields[addressOfField] = value;
}

JrValueHold JrObjectManaged::getField(JrInt addressOfField) {
    assert(fields.size() > addressOfField);
    return fields[addressOfField];
}
