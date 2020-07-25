#include "array.h"

const JrObjectType::Pointer JrArray::Type = JrObjectType::Pointer(new JrObjectType {
    { .kind = JrType_Int, .name = L"Array" }
});

JrFunction::Pointer JrArray_Size::Func;
JrFunction::Pointer JrArray_Append::Func;

void JrArray_Size::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    
}

void JrArray_Append::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    
}

void JrArray::init() {
    JrArray_Size::Func = JrFunction::Pointer(new JrFunction{
        .name = L"Array$$size",
        .kind = jrFuncNative,
        .paramTypes = { JrArray::Type },
        .returnType = JrPrimaryType::Int,
        .nativeCode = new JrArray_Size(),
    });
    
    JrArray_Append::Func = JrFunction::Pointer(new JrFunction {
        .name = L"Array$$append",
        .kind = jrFuncNative,
        .paramTypes = { JrArray::Type, JrType::Any },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrArray_Append()
    });

}
