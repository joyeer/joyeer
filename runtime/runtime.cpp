#include "runtime.h"
#include "gc.h"
#include <cassert>


const JrType::Pointer JrType::Any = std::shared_ptr<JrType>(new JrType{JrType_Any, L"Any"});
const JrType::Pointer JrType::Void = std::shared_ptr<JrType>(new JrType{JrType_Void, L"Void"});
const JrType::Pointer JrType::Nil = std::shared_ptr<JrType>(new JrType{JrType_Nil, L"nil"});

const JrPrimaryType::Pointer JrPrimaryType::Int = std::shared_ptr<JrPrimaryType>(new JrPrimaryType {
    { .kind = JrType_Int, .name = L"Int" }
});

const JrPrimaryType::Pointer JrPrimaryType::Float = std::shared_ptr<JrPrimaryType>(new JrPrimaryType {
    { .kind = JrType_Float, .name = L"Float"}
});

const JrPrimaryType::Pointer JrPrimaryType::Boolean = std::shared_ptr<JrPrimaryType>(new JrPrimaryType {
    {.kind = JrType_Boolean, .name = L"Boolean"}
});


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

