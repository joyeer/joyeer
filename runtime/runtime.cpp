#include "runtime.h"

JrRuntimeStack::JrRuntimeStack() {
    pointer = data;
}

void JrRuntimeStack::push4(uint32_t value) {
    *(uint32_t*)pointer = value;
    pointer += 4;
}

uint32_t JrRuntimeStack::pop4() {
    pointer -= 4;
    return *(uint32_t*)pointer;
}

JrRuntimeContext::JrRuntimeContext() {
    stack = new JrRuntimeStack();
}

JrRuntimeContext::~JrRuntimeContext() {
    delete stack;
}
