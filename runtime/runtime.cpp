#include "runtime.h"
#include "gc.h"
#include <cassert>


JrType* JrType::Any = new JrType{JrType_Any, L"Any"};
JrType* JrType::Void = new JrType{JrType_Void, L"Void"};
JrType* JrType::Nil = new JrType{JrType_Nil, L"nil"};

JrType::JrType(int kind, const std::wstring& name):
kind(kind),
name(name) {
}

JrInt JrType::size() {
    return sizeof(JrInt);
}
////////////////////////////////////////////////////////////////
// JrPrimaryType
////////////////////////////////////////////////////////////////

JrPrimaryType* JrPrimaryType::Int = new JrPrimaryType(JrType_Int, L"Int");
JrPrimaryType* JrPrimaryType::Float = new JrPrimaryType(JrType_Float, L"Float");
JrPrimaryType* JrPrimaryType::Boolean = new JrPrimaryType(JrType_Boolean, L"Boolean");

JrPrimaryType::JrPrimaryType(int kind, const std::wstring& name):
JrType(kind, name) {
}

JrInt JrPrimaryType::size() {
    switch (kind) {
        case JrType_Int:
            return sizeof(JrInt);
        default:
            assert(false);
    }
}

////////////////////////////////////////////////////////////////
// JrObjectType
////////////////////////////////////////////////////////////////

JrObjectType::JrObjectType(const std::wstring& name, JrObjectInitializer initializer, JrObjectFinalizer finalizer):
JrType(JrType_Object, name) {
    this->initializer = initializer;
    this->finalizer = finalizer;
}

void JrObjectType::registerField(JrFieldType::Pointer field) {
    field->addressOfField = virtualFields.size();
    virtualFields.push_back(field);
}

JrInt JrObjectType::size() {
    size_t headSize = sizeof(JrObjectHead);
    size_t size = headSize;
    for(auto field: virtualFields) {
        size += field->type->size();
    }
    
    return size;
}

////////////////////////////////////////////////////////////////
// JrModuleType
////////////////////////////////////////////////////////////////

JrModuleType::JrModuleType(const std::wstring& name):
JrObjectType(name) {
}

JrRuntimeStack::JrRuntimeStack() {
    pointer = data;
}

void JrRuntimeStack::push(JrFunctionFrame::Pointer frame) {
    pointer = frame->endAddress;
    frames.push(frame);
}

void JrRuntimeStack::pop(JrFunctionFrame::Pointer frame) {
    pointer = frame->endAddress;
    assert(frames.top() == frame);
    frames.pop();
}

JrFunctionFrame::Pointer JrRuntimeStack::topFrame() {
    assert(frames.size() > 0);
    return frames.top();
}

void JrRuntimeStack::push(JrInt value) {
    *(JrInt*)pointer = value;
    pointer += sizeof(JrInt);
}

JrInt JrRuntimeStack::pop() {
    pointer -= sizeof(JrInt);
    return *(JrInt*)pointer;
}

void JrRuntimeStack::storeValueForVariable(uint8_t *addressOfVariable, JrInt value) {
    *(JrInt*)addressOfVariable = value;
}

JrInt JrRuntimeStack::intValueOfVariable(uint8_t *addressOfVariable) {
    auto pointer = (int*)addressOfVariable;
    return *pointer;
}

JrRuntimeContext::JrRuntimeContext() {
    stack = std::make_shared<JrRuntimeStack>();
    gc = new JrGC();
    gc->objTable->registerObject((JrObject*)JrObject::nil);
}

JrRuntimeContext::~JrRuntimeContext() {
    delete gc;
}

