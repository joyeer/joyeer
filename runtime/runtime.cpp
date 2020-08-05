#include "runtime.h"
#include "gc.h"
#include <cassert>


JrType* JrType::Any = new JrType{JrType_Any, L"Any"};
JrType* JrType::Void = new JrType{JrType_Void, L"Void"};
JrType* JrType::Nil = new JrType{JrType_Nil, L"nil"};

JrPrimaryType* JrPrimaryType::Int = new JrPrimaryType {
    { .kind = JrType_Int, .name = L"Int" }
};

JrPrimaryType* JrPrimaryType::Float = new JrPrimaryType {
    { .kind = JrType_Float, .name = L"Float"}
};

JrPrimaryType* JrPrimaryType::Boolean = new JrPrimaryType {
    {.kind = JrType_Boolean, .name = L"Boolean"}
};


void JrObjectType::registerField(JrFieldType::Pointer field) {
    field->addressOfField = virtualFields.size();
    virtualFields.push_back(field);
}

JrRuntimeStack::JrRuntimeStack() {
    pointer = data;
}

void JrRuntimeStack::push(JrFunctionFrame::Pointer frame) {
    pointer = frame->endAddress;
    frames.push(frame);
}

void JrRuntimeStack::pop(JrFunctionFrame::Pointer frame) {
    pointer = frame->endAddress;
    assert(frames.top() == frame);
    frames.pop();
}

JrFunctionFrame::Pointer JrRuntimeStack::topFrame() {
    assert(frames.size() > 0);
    return frames.top();
}

void JrRuntimeStack::push(JrInt value) {
    *(JrInt*)pointer = value;
    pointer += sizeof(JrInt);
}

JrInt JrRuntimeStack::pop() {
    pointer -= sizeof(JrInt);
    return *(JrInt*)pointer;
}

void JrRuntimeStack::storeValueForVariable(uint8_t *addressOfVariable, JrInt value) {
    *(JrInt*)addressOfVariable = value;
}

JrInt JrRuntimeStack::intValueOfVariable(uint8_t *addressOfVariable) {
    auto pointer = (int*)addressOfVariable;
    return *pointer;
}

JrRuntimeContext::JrRuntimeContext() {
    stack = std::make_shared<JrRuntimeStack>();
    gc = new JrGC();
    gc->objTable->registerObject((JrObject*)JrObject::nil);
}

JrRuntimeContext::~JrRuntimeContext() {
    delete gc;
}

