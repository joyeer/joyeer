#include "print.h"
#include "runtime/gc.h"
#include "runtime/buildin.h"
#include "runtime/sys/array.h"
#include "runtime/sys/string.h"
#include <iostream>


JrFunction::Pointer JrFuncPrint::Func;

void JrFuncPrint::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    auto value = context->stack->pop();
    switch (value.kind) {
        case typeString: {
            auto stringObjRef = value.intValue;
            auto stringObj = (JrObjectString*)context->gc->get(stringObjRef);
            std::wcout << *(stringObj->content) << std::endl;
        }
            break;
        case typeObject: {
            std::wcout << L"[object] #" << value.intValue << std::endl;
        }
            break;
        case typeNil: {
            std::wcout << L"[nil] #" << value.intValue << std::endl;
        }
            break;
        default: {
            std::wcout << value.intValue << std::endl;
        }
            break;
    }
    
};

void JrFuncPrint::init() {
    JrFuncPrint::Func = std::shared_ptr<JrFunction>(new JrFunction {
        .name = L"print(message:)",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrType::Any },
        .nativeCode = new JrFuncPrint()
    });

    Global::registerFunction(JrFuncPrint::Func, nullptr);
}

JrFunction::Pointer JrFuncObjectPrint::Func;

void JrFuncObjectPrint::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    auto objectRef = context->stack->pop();
    
    auto object = context->gc->get(objectRef.objRefValue);
    auto arrayObject = static_cast<JrObjectArray*>(object);
    
    std::wcout << L"[";
    for(int i = 0; i < arrayObject->slots->size(); i ++) {
        if(i > 0) {
            std::wcout << L", ";
        }
        std::wcout << (*arrayObject->slots)[i].intValue;
        
    }
    std::wcout << L"]" << std::endl;
}

void JrFuncObjectPrint::init() {
    JrFuncObjectPrint::Func = std::shared_ptr<JrFunction>(new JrFunction {
        .name = L"print(object:)",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrType::Any },
        .nativeCode = new JrFuncObjectPrint()
    });
    
    Global::registerFunction(JrFuncObjectPrint::Func, nullptr);
}
