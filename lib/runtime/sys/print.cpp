#include "joyeer/runtime/sys/print.h"
#include "joyeer/runtime/buildin.h"
#include "joyeer/runtime/gc.h"
#include "joyeer/runtime/sys/array.h"
#include "joyeer/runtime/sys/string.h"
#include <iostream>


JrFunction* JrFuncPrint::Func;

void JrFuncPrint::operator()(JrRuntimeContext* context, JrFunction* func) {
    auto value = context->stack->pop();
    switch (value.kind) {
        case typeString: {
            auto stringObjRef = value.intValue;
            auto stringObj = (JrObjectString*)context->gc->get(stringObjRef);
            std::cout << *(stringObj->content) << std::endl;
        }
            break;
        case typeObject: {
            std::cout << "[object] #" << value.intValue << std::endl;
        }
            break;
        case typeNil: {
            std::cout << "[nil] #" << value.intValue << std::endl;
        }
            break;
        default: {
            std::cout << value.intValue << std::endl;
        }
            break;
    }
    
};

void JrFuncPrint::init() {
    JrFuncPrint::Func = new JrFunction {
        .name = "print(message:)",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrType::Any },
        .nativeCode = new JrFuncPrint()
    };

    Global::registerFunction(JrFuncPrint::Func, nullptr);
}

JrFunction* JrFuncObjectPrint::Func;

void JrFuncObjectPrint::operator()(JrRuntimeContext* context, JrFunction* func) {
    auto objectRef = context->stack->pop();
    
    auto object = context->gc->get(objectRef.objRefValue);
    auto arrayObject = static_cast<JrObjectArray*>(object);
    
    std::cout << "[";
    for(int i = 0; i < arrayObject->slots->size(); i ++) {
        if(i > 0) {
            std::cout << ", ";
        }
        std::cout << (*arrayObject->slots)[i].intValue;
        
    }
    std::cout << "]" << std::endl;
}

void JrFuncObjectPrint::init() {
    JrFuncObjectPrint::Func = new JrFunction {
        .name = "print(object:)",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrType::Any },
        .nativeCode = new JrFuncObjectPrint()
    };
    
    Global::registerFunction(JrFuncObjectPrint::Func, nullptr);
}
