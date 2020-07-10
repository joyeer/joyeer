#include "runtime.h"
#include <cassert>

const JrType::Pointer JrType::Any = std::make_shared<JrType>(JrType{JrType_Any, L"Any"});
const JrPrimaryType::Pointer JrPrimaryType::Int = std::shared_ptr<JrPrimaryType>(new JrPrimaryType {
    { .kind = JrType_Int, .name = L"Int" }
});

const JrPrimaryType::Pointer JrPrimaryType::Float = std::shared_ptr<JrPrimaryType>(new JrPrimaryType {
    { .kind = JrType_Float, .name = L"Float"}
});

const JrPrimaryType::Pointer JrPrimaryType::Boolean = std::shared_ptr<JrPrimaryType>(new JrPrimaryType {
    {.kind = JrType_Boolean, .name = L"Boolean"}
});

JrRuntimeStack::JrRuntimeStack() {
    pointer = data;
}

void JrRuntimeStack::push(JrFunctionFrame::Pointer frame) {
    pointer = frame->endAddress;
}

void JrRuntimeStack::pop(JrFunctionFrame::Pointer frame) {
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

void JrRuntimeStack::restore(uint8_t *address) {
    pointer = address;
}

void JrRuntimeStack::storeValueForVariable(uint8_t *addressOfVariable, int value) {
    *(int*)addressOfVariable = value;
}

int JrRuntimeStack::intValueOfVariable(uint8_t *addressOfVariable) {
    auto pointer = (int*)addressOfVariable;
    return *pointer;
}

JrRuntimeContext::JrRuntimeContext() {
    stack = std::make_shared<JrRuntimeStack>();
}

JrRuntimeContext::~JrRuntimeContext() {
}

