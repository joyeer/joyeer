#include "print.h"
#include <iostream>

JrFunction::Pointer JrFuncPrint::Func;

void JrFuncPrint::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    auto pointer = context->stack->pointer;
    uint32_t value = context->stack->pop();
    std::cout << value << std::endl;
    context->stack->pointer = pointer;
};

void JrFuncPrint::init() {
    JrFuncPrint::Func = std::shared_ptr<JrFunction>(new JrFunction {
        .name = L"print(message:)",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrType::Any },
        .nativeCode = new JrFuncPrint()
    });

}
