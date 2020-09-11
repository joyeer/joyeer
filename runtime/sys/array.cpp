#include "array.h"
#include "runtime/gc.h"
#include "runtime/object.h"
#include "runtime/buildin.h"


auto JrObjectIntArrayInitializer = [](JrPtr memory) -> JrObject* {
    return new(memory) JrObjectArray;
};

auto JrObjectIntArrayFinalizer = [](JrObject* object) {
    auto arrayObject = (JrObjectArray*)object;
    arrayObject->~JrObjectArray();
};

struct JrObjectIntArrayType : public JrObjectType {
    JrObjectIntArrayType():
        JrObjectType(L"Array", JrObjectIntArrayInitializer, JrObjectIntArrayFinalizer) {
    }
    
    JrInt size() {
        return sizeof(JrObjectArray);
    }
};

JrObjectType* JrObjectArray::Type = new JrObjectIntArrayType();
JrFunction* JrObjectArray::Constructor;
JrFunction* JrObjectArray_Size::Func;
JrFunction* JrObjectArray_Append::Func;
JrFunction* JrObjectArray_Get::Func;
JrFunction* JrObjectArray_Set::Func;

JrObjectArray::JrObjectArray():
slots(new std::vector<JrValueHold>()) {
}

JrObjectArray::~JrObjectArray() {
    delete slots;
}

void JrObjectArray_Size::operator()(JrRuntimeContext::Pointer context, JrFunction* func) {
    auto objectRef = context->stack->pop();
    auto object = context->gc->get(objectRef.objRefValue);
    auto arrayObject = static_cast<JrObjectArray*>(object);
    
    context->stack->push({.kind = typeInt, .intValue = static_cast<JrInt>(arrayObject->slots->size())});
}

void JrObjectArray_Append::operator()(JrRuntimeContext::Pointer context, JrFunction* func) {
    
}

void JrObjectArray_Get::operator()(JrRuntimeContext::Pointer context, JrFunction* func) {
    auto objectRef = context->stack->pop();
    auto arrayIndex = context->stack->pop();
    
    auto object = context->gc->get(objectRef.objRefValue);
    auto arrayObject = static_cast<JrObjectArray*>(object);
    
    auto value = (*arrayObject->slots)[arrayIndex.intValue];
    context->stack->push(value);
}

void JrObjectArray_Set::operator()(JrRuntimeContext::Pointer context, JrFunction* func) {
    auto value = context->stack->pop();
    auto arrayIndex = context->stack->pop();
    auto objectRef = context->stack->pop();
    
    
    auto object = context->gc->get(objectRef.objRefValue);
    auto arrayObject = static_cast<JrObjectArray*>(object);
    (*arrayObject->slots)[arrayIndex.intValue] = value;
    
}

void JrObjectArray::init() {
    
    JrObjectArray::Constructor = new JrFunction {
        .name = L"Array@Array()",
        .kind = jrFuncConstructor,
        .paramTypes = { JrObjectArray::Type },
        .localVars = { JrVar {
            .name = L"self",
            .type = JrObjectArray::Type,
            .addressOfVariable = 0
        }},
        .returnType = JrObjectArray::Type,
        .instructions = {
                         { .opcode = OP_OLOAD, .value = 0 },
                         { .opcode = OP_ORETURN, .value = 0 }
        }
    };
    
    JrObjectArray_Size::Func = new JrFunction {
        .name = L"Array@size()",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectArray::Type },
        .returnType = JrPrimaryType::Int,
        .nativeCode = new JrObjectArray_Size(),
    };
    
    JrObjectArray_Append::Func = new JrFunction {
        .name = L"Array@append(value:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectArray::Type, JrType::Any },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectArray_Append()
    };
    
    JrObjectArray_Get::Func = new JrFunction {
        .name = L"Array@get(index:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectArray::Type, JrPrimaryType::Int },
        .returnType = JrPrimaryType::Int,
        .nativeCode = new JrObjectArray_Get()
    };
    
    JrObjectArray_Set::Func = new JrFunction {
        .name = L"Array@set(index:value:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectArray::Type, JrPrimaryType::Int },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectArray_Set()
    };
    
    Global::registerObjectType(JrObjectArray::Type);
    Global::registerFunction(JrObjectArray::Constructor, JrObjectArray::Type);
    Global::registerFunction(JrObjectArray_Append::Func, JrObjectArray::Type);
    Global::registerFunction(JrObjectArray_Size::Func, JrObjectArray::Type);
    Global::registerFunction(JrObjectArray_Get::Func, JrObjectArray::Type);
    Global::registerFunction(JrObjectArray_Set::Func, JrObjectArray::Type);
    
}
