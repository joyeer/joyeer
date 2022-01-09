//
// Created by Qing Xu on 2021/11/26.
//
#include "joyeer/runtime/types.h"
#include "joyeer/runtime/isolate+vm.h"

#include <sstream>
#include <functional>

std::string debugAccessFlag(AccessFlag flag) {
    bool one = false;
    std::stringstream  ss;

#define ACCESS_FLAGS_PRINT(name) \
    if(one) { \
        ss << " "; \
    } \
    ss << (name); \
    one = true;

    if((flag & AccessFlag::Public) == AccessFlag::Public) {
        ACCESS_FLAGS_PRINT("public")
    }

    if((flag & AccessFlag::Protected) == AccessFlag::Protected) {
        ACCESS_FLAGS_PRINT("protected")
    }

    if((flag & AccessFlag::Private) == AccessFlag::Private) {
        ACCESS_FLAGS_PRINT("private")
    }

    if((flag & AccessFlag::Static) == AccessFlag::Static) {
        ACCESS_FLAGS_PRINT("static")
    }

    return ss.str();
}

NilType::NilType():
        Type("nil", ValueType::Nil){}

VoidType::VoidType() :
        Type("void", ValueType::Void) {}

IntType::IntType() :
        Type("Int", ValueType::Int) {}

BoolType::BoolType() :
        Type("Bool", ValueType::Bool) {}

BlockType::BlockType() :
        Type("Block", ValueType::Block),
        localVars() {}

Function::Function(const std::string &name, bool isStatic) :
        Type(name, ValueType::Func),
        isStatic(isStatic),
        funcKind(FuncTypeKind::VM_Func) {
}

int Function::getLocalVarCount() const {
    return localVars.size();
}

//------------------------------------------------
// Class implementation
//------------------------------------------------

Class::Class(const std::string &name):
    Type(name, ValueType::Class){
}

intptr_t Class::allocate(IsolateVM *vm) {
    assert(false);
    return 0;
}

//------------------------------------------------
// Class implementation
//------------------------------------------------

ModuleClass::ModuleClass(const std::string &name) :
        Class(name) {
    kind = ValueType::Module;
}

//------------------------------------------------
// ArrayClass implementation
//------------------------------------------------
ArrayClass::ArrayClass():
Class(std::string("Array")) {
}


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

//------------------------------------------------
// DictClass implementation
//------------------------------------------------

DictClass::DictClass():
Class(std::string("Array")) {

}

intptr_t DictClass::allocate(IsolateVM *vm) {
    this->vm = vm;
    int defaultSize = kDictSpaceSizeOffset + kDefaultDictSize / 4 * kIntSize;
    auto objPtr= vm->gc->allocate(this, defaultSize);
    setCapacity(objPtr, {.intValue = kDefaultDictSize} );
    setSize(objPtr, {.intValue = 0});
    return objPtr;
}

Value DictClass::capacity(intptr_t object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kDictCapacityOffset);
}

void DictClass::setCapacity(intptr_t object, Value capacity) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kDictCapacityOffset) = capacity;
}

Value DictClass::getSpaceSize(intptr_t object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kDictCapacityOffset);
}

Value DictClass::getSpaceSlot(intptr_t object, Slot slot) {
    char* objPtr = reinterpret_cast<char *>(object);
    return ((Value*)(objPtr + kDictSpaceSizeOffset))[slot];
}

void DictClass::setSpaceSlot(intptr_t object, Slot slot, Value value) {
    char* objPtr = reinterpret_cast<char *>(object);
    ((Value*)(objPtr + kDictSpaceSizeOffset))[slot] = value;
}

Value DictClass::size(intptr_t object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kDictSizeOffset);
}

void DictClass::setSize(intptr_t object, Value value) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kDictSizeOffset) = value;
}

void DictClass::insert(intptr_t object, Value key, Value value) {
    auto hash = std::hash<Int>{}(key.intValue);
    auto capacityValue = capacity(object);
    auto spaceSize = getSpaceSize(object);
    auto pos = (hash % spaceSize.intValue);

    auto spaceSlot = getSpaceSlot(object, (Slot)pos);

    if(spaceSlot.intValue == 0){
        // slot is empty, let's create a bucket
        auto bucket = vm->arrayClass->allocate(vm, 4);
        setSpaceSlot(object, spaceSlot.intValue, {.intValue = bucket} );
    }

    auto bucket = getSpaceSlot(object, spaceSlot.slotValue);
    auto countOfItemBucket = vm->arrayClass->getLength(bucket.intValue);
    bool founded = false;
    for(auto i = 0; i < countOfItemBucket.intValue; i ++ ) {
        auto itemEntryObj = vm->arrayClass->get(bucket.intValue, {.intValue = i});
        auto entryKey = vm->dictEntryClass->getKey(itemEntryObj.intValue);

        if(entryKey.intValue == key.intValue) {
            founded = true;
            vm->dictEntryClass->setValue(itemEntryObj.intValue, value);
        }
    }

    if(!founded) {
        // create entry in bucket
        auto entryObj = vm->dictEntryClass->allocate(vm);
        vm->dictEntryClass->setKey(entryObj, key);
        vm->dictEntryClass->setValue(entryObj, value);
        vm->arrayClass->append(bucket.intValue,{.intValue = entryObj });
    }

}

Value DictClass::get(intptr_t object, Value key) {
    assert(false);
}

//------------------------------------------------
// DictEntry implementation
//------------------------------------------------

DictEntry::DictEntry():
Class(std::string("DictEntry")) {
}

intptr_t DictEntry::allocate(IsolateVM *vm) {
    intptr_t object = vm->gc->allocate(this, kDictEntryValueOffset );
    return object;
}

Value DictEntry::getKey(intptr_t object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kDictEntryKeyOffset);
}

void DictEntry::setKey(intptr_t object, Value keyValue) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kDictEntryKeyOffset) = keyValue;
}

Value DictEntry::getValue(intptr_t object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kDictEntryValueOffset);
}

void DictEntry::setValue(intptr_t object, Value valueValue) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kDictEntryValueOffset) = valueValue;
}