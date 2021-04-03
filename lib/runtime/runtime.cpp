#include "joyeer/runtime/runtime.h"
#include "joyeer/runtime/gc.h"
#include <cassert>


JrType* JrType::Any = new JrType{typeAny, "Any"};
JrType* JrType::Void = new JrType{typeVoid, "Void"};
JrType* JrType::Nil = new JrType{typeNil, "nil"};

JrType::JrType(JrTypeKind kind, const std::string& name):
kind(kind),
name(name) {
}

JrInt JrType::size() {
    return sizeof(JrInt);
}
////////////////////////////////////////////////////////////////
// JrPrimaryType
////////////////////////////////////////////////////////////////

JrPrimaryType* JrPrimaryType::Int = new JrPrimaryType(typeInt, "Int");
JrPrimaryType* JrPrimaryType::Float = new JrPrimaryType(typeFloat, "Float");
JrPrimaryType* JrPrimaryType::Boolean = new JrPrimaryType(typeBoolean, "Boolean");

JrPrimaryType::JrPrimaryType(JrTypeKind kind, const std::string& name):
JrType(kind, name) {
}

JrInt JrPrimaryType::size() {
    switch (kind) {
        case typeInt:
            return sizeof(JrInt);
        default:
            assert(false);
    }
}

////////////////////////////////////////////////////////////////
// JrObjectType
////////////////////////////////////////////////////////////////

JrObjectType::JrObjectType(const std::string& name, JrObjectInitializer initializer, JrObjectFinalizer finalizer):
JrType(typeObject, name) {
    this->initializer = initializer;
    this->finalizer = finalizer;
}

void JrObjectType::registerField(JrFieldType::Ptr field) {
    field->addressOfField = virtualFields.size();
    virtualFields.push_back(field);
}

JrInt JrObjectType::size() {
    return sizeof(JrObjectManaged);
}

////////////////////////////////////////////////////////////////
// JrModuleType
////////////////////////////////////////////////////////////////

JrModuleClass::JrModuleClass(const std::string& name):
JrObjectType(name) {
}

JrRuntimeStack::JrRuntimeStack() {
    pointer = 0;
}

void JrRuntimeStack::startFuncCall(JrFunctionFrame::Ptr frame) {
    pointer = frame->endAddress;
    frames.push(frame);
}

void JrRuntimeStack::endFuncCall(JrFunctionFrame::Ptr frame) {
    pointer = frame->endAddress;
    assert(frames.top() == frame);
    frames.pop();
}

JrFunctionFrame::Ptr JrRuntimeStack::topFrame() {
    assert(frames.size() > 0);
    return frames.top();
}

JrValueHold JrRuntimeStack::top() {
    return *(JrValueHold*)(data + pointer - sizeof(JrValueHold));
}

void JrRuntimeStack::push(JrValueHold value) {
    *(JrValueHold*)(data + pointer) = value;
    pointer += sizeof(JrValueHold);
}

JrValueHold JrRuntimeStack::pop() {
    pointer -= sizeof(JrValueHold);
    return *(JrValueHold*)(data + pointer);
}

void JrRuntimeStack::storeValueForVariable(JrInt addressOfVariable, JrValueHold value) {
    *(JrValueHold*)(data + addressOfVariable) = value;
}

JrValueHold JrRuntimeStack::intValueOfVariable(JrInt addressOfVariable) {
    auto pointer = (JrValueHold*)(data + addressOfVariable);
    return *pointer;
}

JrRuntimeContext::JrRuntimeContext() {
    stack = new JrRuntimeStack();
    gc = new JrGC();
    gc->objTable->registerObject((JrObject*)JrObject::Nil);
}

JrRuntimeContext::~JrRuntimeContext() {
    delete gc;
    delete stack;
}
