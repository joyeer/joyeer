//
// Created by Qing Xu on 2021/11/4.
//
#include "joyeer/vm/types.h"
#include "joyeer/vm/bytecode.h"
#include "joyeer/vm/interpreter.h"

VMethod::VMethod(Bytecodes* bytecodes, int paramCount, int localVarCount):
Method(MethodKind::VM_Method),
paramCount(paramCount),
localVarCount(localVarCount),
bytecodes(bytecodes) {
}

VMethod::~VMethod() {
    delete(bytecodes);
}

Value VMethod::operator()(Executor* executor, Arguments *args) const {
    Value result;
    return result;
}

////////////////////////////////////////////////
// ArrayClass implementation
////////////////////////////////////////////////
intptr_t ArrayClass::allocate(IsolateVM* vm, int capacity) {
    size_t adjustedCapacity = calculateArrayCapacitySize(capacity) * kIntSize;
    size_t size = adjustedCapacity + kArrayDataOffset;

    intptr_t object = vm->gc->allocate(this, size);
    setCapacity(object, {.intValue = static_cast<Int>(adjustedCapacity) });

    return object;
}

size_t ArrayClass::calculateArrayCapacitySize(int size) {
    size_t result = 2;
    while(result < size) {
        result *= 2;
    }
    return result;
}

void ArrayClass::setCapacity(intptr_t object, Value capacity) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kArrayCapacityOffset) = capacity;
}

Value ArrayClass::getCapacity(intptr_t object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kArrayCapacityOffset);
}

void ArrayClass::setLength(intptr_t object, Value length) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kArrayLengthOffset) = length;
}

Value ArrayClass::getLength(intptr_t object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kArrayLengthOffset);
}

void ArrayClass::append(intptr_t object, Value value) {
    char* objPtr = reinterpret_cast<char *>(object);
    auto lengthValue = ArrayClass::getLength(object);
    *(Value*)(objPtr + kArrayDataOffset + kIntSize * (lengthValue.intValue)) = value;
    ArrayClass::setLength(object, {.intValue = lengthValue.intValue + 1});
}

Value ArrayClass::get(intptr_t object, Value index) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kArrayDataOffset + kIntSize * (index.intValue));
}

void ArrayClass::set(intptr_t object, Value index, Value value) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kArrayDataOffset + kIntSize * (index.intValue)) = value;
}