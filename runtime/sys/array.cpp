#include "array.h"

const JrObjectType::Pointer JrObjectIntArray::Type = JrObjectType::Pointer(new JrObjectType {
    { .kind = JrType_Int, .name = L"IntArray" }
});

JrFunction::Pointer JrObjectIntArray_Size::Func;
JrFunction::Pointer JrObjectIntArray_Append::Func;

void JrObjectIntArray_Size::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    
}

void JrObjectIntArray_Append::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    
}

void JrObjectIntArray::init() {
    JrObjectIntArray_Size::Func = JrFunction::Pointer(new JrFunction{
        .name = L"Array@size",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectIntArray::Type },
        .returnType = JrPrimaryType::Int,
        .nativeCode = new JrObjectIntArray_Size(),
    });
    
    JrObjectIntArray_Append::Func = JrFunction::Pointer(new JrFunction {
        .name = L"Array@append",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectIntArray::Type, JrType::Any },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectIntArray_Append()
    });

}
