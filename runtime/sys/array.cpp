#include "array.h"
#include "runtime/gc.h"
#include "runtime/object.h"

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
        .name = L"Array@size",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrObjectIntArray::Type },
        .returnType = (JrType*)JrPrimaryType::Int,
        .nativeCode = new JrObjectIntArray_Size(),
    });
    
    JrObjectIntArray_Append::Func = JrFunction::Pointer(new JrFunction {
        .name = L"Array@append",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrObjectIntArray::Type, (JrType*)JrType::Any },
        .returnType = (JrType*)JrPrimaryType::Void,
        .nativeCode = new JrObjectIntArray_Append()
    });
    
    JrObjectIntArray_Get::Func = JrFunction::Pointer(new JrFunction {
        .name = L"Array@get",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrObjectIntArray::Type, (JrType*)JrPrimaryType::Int },
        .returnType = (JrType*)JrPrimaryType::Int,
        .nativeCode = new JrObjectIntArray_Get()
    });
    
    JrObjectIntArray_Set::Func = JrFunction::Pointer(new JrFunction {
        .name = L"Array@set",
        .kind = jrFuncNative,
        .paramTypes = { (JrType*)JrObjectIntArray::Type, (JrType*)JrPrimaryType::Int },
        .returnType = (JrType*)JrPrimaryType::Void,
        .nativeCode = new JrObjectIntArray_Set()
    });
}
