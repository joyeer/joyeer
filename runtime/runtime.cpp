#include "runtime.h"
#include <cassert>

const JrType::Pointer JrType::Any = std::make_shared<JrType>(JrType{JrType_Any, L"Any"});
const JrPrimaryType::Pointer JrPrimaryType::Int = std::shared_ptr<JrPrimaryType>(new JrPrimaryType {
    { .kind = JrType_Int, .name = L"Int" }
});

const JrPrimaryType::Pointer JrPrimaryType::Float = std::shared_ptr<JrPrimaryType>(new JrPrimaryType {
    { .kind = JrType_Float, .name = L"Float"}
});

JrRuntimeStack::JrRuntimeStack() {
    pointer = data;
}

void JrRuntimeStack::push(JrFunctionFrame::Pointer frame) {
    assert(frame->startAddress == pointer);
    pointer = frame->endAddress;
}

void JrRuntimeStack::push4(uint32_t value) {
    *(uint32_t*)pointer = value;
    pointer += 4;
}

uint32_t JrRuntimeStack::pop4() {
    pointer -= 4;
    return *(uint32_t*)pointer;
}

void JrRuntimeStack::storeValueForVariable(uint8_t *addressOfVariable, int value) {
    assert(addressOfVariable < pointer);
    *(int*)addressOfVariable = value;
}

int JrRuntimeStack::intValueOfVariable(uint8_t *addressOfVariable) {
    auto pointer = (int*)addressOfVariable;
    return *pointer;
}

JrRuntimeContext::JrRuntimeContext() {
    stack = new JrRuntimeStack();
}

JrRuntimeContext::~JrRuntimeContext() {
    delete stack;
}

