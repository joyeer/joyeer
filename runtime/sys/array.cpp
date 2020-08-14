#include "array.h"
#include "runtime/gc.h"
#include "runtime/object.h"
#include "runtime/buildin.h"


auto JrObjectIntArrayInitializer = [](JrPtr memory) -> JrObject* {
    return new(memory) JrObjectIntArray;
};

auto JrObjectIntArrayFinalizer = [](JrObject* object) {
    auto arrayObject = (JrObjectIntArray*)object;
    arrayObject->~JrObjectIntArray();
};

struct JrObjectIntArrayType : public JrObjectType {
    JrObjectIntArrayType():
        JrObjectType(L"Array<Int>", JrObjectIntArrayInitializer, JrObjectIntArrayFinalizer) {
    }
    
    JrInt size() {
        return sizeof(JrObjectIntArray);
    }
};

JrObjectType* JrObjectIntArray::Type = new JrObjectIntArrayType();


JrObjectIntArray::JrObjectIntArray():
slots(new std::vector<JrInt>()) {
}

JrObjectIntArray::~JrObjectIntArray() {
    delete slots;
}

JrFunction::Pointer JrObjectIntArray_Size::Func;
JrFunction::Pointer JrObjectIntArray_Append::Func;
JrFunction::Pointer JrObjectIntArray_Get::Func;
JrFunction::Pointer JrObjectIntArray_Set::Func;

void JrObjectIntArray_Size::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    auto objectRef = context->stack->pop();
    auto arrayIndex = context->stack->pop();
    auto object = context->gc->get(objectRef);
    auto arrayObject = static_cast<JrObjectIntArray*>(object);
    
    context->stack->push(arrayObject->slots->size());
}

void JrObjectIntArray_Append::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    
}

void JrObjectIntArray_Get::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    auto objectRef = context->stack->pop();
    auto arrayIndex = context->stack->pop();
    
    auto object = context->gc->get(objectRef);
    auto arrayObject = static_cast<JrObjectIntArray*>(object);
    
    auto value = (*arrayObject->slots)[arrayIndex];
    context->stack->push(value);
}

void JrObjectIntArray_Set::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    auto objectRef = context->stack->pop();
    auto arrayIndex = context->stack->pop();
    
    auto value = context->stack->pop();
    
    auto object = context->gc->get(objectRef);
    auto arrayObject = static_cast<JrObjectIntArray*>(object);
    (*arrayObject->slots)[arrayIndex] = value;
    
}

void JrObjectIntArray::init() {
    JrObjectIntArray_Size::Func = JrFunction::Pointer(new JrFunction{
        .name = L"Array<Int>@size()",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectIntArray::Type },
        .returnType = JrPrimaryType::Int,
        .nativeCode = new JrObjectIntArray_Size(),
    });
    
    JrObjectIntArray_Append::Func = JrFunction::Pointer(new JrFunction {
        .name = L"Array<Int>@append(value:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectIntArray::Type, JrType::Any },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectIntArray_Append()
    });
    
    JrObjectIntArray_Get::Func = JrFunction::Pointer(new JrFunction {
        .name = L"Array<Int>@get(index:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectIntArray::Type, JrPrimaryType::Int },
        .returnType = JrPrimaryType::Int,
        .nativeCode = new JrObjectIntArray_Get()
    });
    
    JrObjectIntArray_Set::Func = JrFunction::Pointer(new JrFunction {
        .name = L"Array<Int>@set(index:value:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectIntArray::Type, JrPrimaryType::Int },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectIntArray_Set()
    });
    
    Global::registerObjectType(JrObjectIntArray::Type);
    Global::registerFunction(JrObjectIntArray_Append::Func, JrObjectIntArray::Type);
    Global::registerFunction(JrObjectIntArray_Size::Func, JrObjectIntArray::Type);
    Global::registerFunction(JrObjectIntArray_Get::Func, JrObjectIntArray::Type);
    Global::registerFunction(JrObjectIntArray_Set::Func, JrObjectIntArray::Type);
    
}
