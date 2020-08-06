#include "array.h"
#include "runtime/gc.h"

JrObjectType* JrObjectIntArray::Type = new JrObjectType(L"IntArray");

JrFunction::Pointer JrObjectIntArray_Size::Func;
JrFunction::Pointer JrObjectIntArray_Append::Func;
JrFunction::Pointer JrObjectIntArray_Get::Func;

void JrObjectIntArray_Size::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    
}

void JrObjectIntArray_Append::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    
}

void JrObjectIntArray_Get::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    auto objectRef = context->stack->pop();
    auto arrayIndex = context->stack->pop();
    
    auto object = context->gc->get(objectRef);
    auto arrayObject = static_cast<JrObjectIntArray*>(object);
    
    auto value = arrayObject->slots[arrayIndex];
    
    context->stack->pop(context->stack->topFrame());
    context->stack->push(value);
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
}
