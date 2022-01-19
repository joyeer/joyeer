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

Function::Function(const std::string &name, bool isStatic) :
        Type(name, ValueType::Func),
        isStatic(isStatic),
        cFunction(nullptr),
        funcKind(FuncTypeKind::VM_Func) {
}

int Function::getLocalVarCount() const {
    return (int)localVars.size();
}

Variable *Function::getParamByIndex(int index) {
    return localVars[index];
}


//------------------------------------------------
// Optional implementation
//------------------------------------------------

Optional::Optional():
Type("Optional", ValueType::Any) {
}

intptr_t Optional::allocate(IsolateVM *vm, Int value) {
    auto objAddr = vm->gc->allocate(this, sizeof(DataMap));
    auto objPtr = reinterpret_cast<DataMap*>(objAddr);
    objPtr->head.wrapped = true;
    objPtr->head.absent = false;
    objPtr->head.typeSlot = (*vm->types)[ValueType::Int]->slot;
    objPtr->wrappedValue = value;

    return objAddr;
}

intptr_t Optional::allocate(IsolateVM *vm, Bool value) {
    auto objAddr = vm->gc->allocate(this, sizeof(DataMap));
    auto objPtr = reinterpret_cast<DataMap*>(objAddr);
    objPtr->head.wrapped = true;
    objPtr->head.absent = false;
    objPtr->head.typeSlot = (*vm->types)[ValueType::Bool]->slot;
    objPtr->wrappedValue = value;
    return objAddr;
}

Slot Optional::valueType(intptr_t objAddr) {
    auto optionalObj = reinterpret_cast<DataMap*>(objAddr);
    if(optionalObj == nullptr) {
        return (Slot)ValueType::Nil;
    }
    return optionalObj->head.typeSlot;
}

Int Optional::intValue(intptr_t object) {
    auto optionalObj = reinterpret_cast<DataMap*>(object);
    assert(optionalObj->head.typeSlot == (Int)ValueType::Int);
    return optionalObj->wrappedValue;
}

Bool Optional::boolValue(intptr_t objAddr) {
    auto optionalObj = reinterpret_cast<DataMap*>(objAddr);
    assert(optionalObj->head.typeSlot == (Int)ValueType::Bool);
    return optionalObj->wrappedValue;
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
// Module implementation
//------------------------------------------------

ModuleClass::ModuleClass(const std::string &name) :
        Class(name) {
    kind = ValueType::Module;
}

//------------------------------------------------
// StringClass implementation
//------------------------------------------------

intptr_t StringClass::allocate(IsolateVM *vm, int typeSlot) {

    auto rawString = (*vm->strings)[typeSlot];
    auto objAddr = vm->gc->allocate(this, (int)(rawString.size() + kIntSize + sizeof(ObjectHead) + kIntSize));
    auto stringObj = reinterpret_cast<StringData*>(objAddr);
    stringObj->head.typeSlot = (Int)ValueType::String;
    stringObj->length = static_cast<Int>(rawString.size());
    memcpy(stringObj->data, rawString.c_str(), stringObj->length);

    return objAddr;
}

intptr_t StringClass::allocateWithLength(IsolateVM *vm, int size) {
    auto objAddr = vm->gc->allocate(this, (int)(size + kIntSize + sizeof(ObjectHead) + kIntSize));
    auto stringObj = reinterpret_cast<StringData*>(objAddr);
    stringObj->head.typeSlot = (Int)ValueType::String;
    stringObj->length = (Int)size;

    return objAddr;
}

std::string StringClass::toString(intptr_t objAddr) {
    auto stringObj = reinterpret_cast<StringData*>(objAddr);
    return {stringObj->data, (size_t )stringObj->length };
}

int StringClass::getLength(intptr_t thisAddr) {
    auto stringObj = reinterpret_cast<StringData*>(thisAddr);
    return stringObj->length;
}

intptr_t StringClass::c_str(intptr_t thisAddr) {
    auto stringObj = reinterpret_cast<StringData*>(thisAddr);
    return reinterpret_cast<intptr_t>(stringObj->data);
}

//------------------------------------------------
// StringBuilderClass implementation
//------------------------------------------------
intptr_t StringBuilderClass::allocate(IsolateVM *vm) {
    auto sbAddr = vm->gc->allocate(this, sizeof(StringBuilderData));
    auto sbObj = reinterpret_cast<StringBuilderData*>(sbAddr);
    auto arrayObj = vm->arrayClass->allocate(vm, 8);
    sbObj->head.typeSlot = (int)BuildIns::Object_StringBuilder;
    sbObj->stringArray = arrayObj;

    return sbAddr;
}

intptr_t StringBuilderClass::append(intptr_t thisAddr, intptr_t stringAddr) {
    auto thisObj = reinterpret_cast<StringBuilderData*>(thisAddr);;
    vm->arrayClass->append(thisObj->stringArray, stringAddr);
    return thisAddr;
}

intptr_t StringBuilderClass::toString(intptr_t thisAddr) {
    auto thisObj = reinterpret_cast<StringBuilderData*>(thisAddr);
    int stringLength = 0;
    auto arrayLength = vm->arrayClass->getLength(thisObj->stringArray);
    for(int i = 0 ; i < arrayLength; i ++) {
        auto arrayObj =  vm->arrayClass->get(thisObj->stringArray, i);
        auto length = vm->stringClass->getLength(arrayObj);
        stringLength += length;
    }

    auto resultStringAddr = vm->stringClass->allocateWithLength(vm, stringLength);
    auto resultStringObj = reinterpret_cast<StringClass::StringData*>(resultStringAddr);
    auto startIndex = 0;
    for(int i = 0 ; i < arrayLength; i ++) {
        auto srcStringObj =  vm->arrayClass->get(thisObj->stringArray, arrayLength - i - 1);

        auto cStr = vm->stringClass->c_str(srcStringObj);
        auto srcLen = vm->stringClass->getLength(srcStringObj);
        memcpy((char*)(resultStringObj->data + startIndex), (char*)cStr, srcLen);
        startIndex += srcLen;
    }

    return resultStringAddr;
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

    intptr_t object = vm->gc->allocate(this, size + kIntSize);
    setCapacity(object, (Value)adjustedCapacity);

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
    auto lengthValue = getLength(object);
    *(Value*)(objPtr + kArrayDataOffset + kValueSize * lengthValue) = value;
    setLength(object, lengthValue + 1);
}

Value ArrayClass::get(intptr_t object, Value index) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kArrayDataOffset + kValueSize * index);
}

void ArrayClass::set(intptr_t object, Value index, Value value) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kArrayDataOffset + kValueSize * index) = value;
}

//------------------------------------------------
// DictClass implementation
//------------------------------------------------

DictClass::DictClass():
Class(std::string("Array")) {

}

intptr_t DictClass::allocate(IsolateVM *vm) {
    this->vm = vm;
    int defaultSize = kDictBucketSlotOffset + kDefaultDictSize / 4 * kIntSize;
    auto objPtr= vm->gc->allocate(this, defaultSize + kIntSize) ;
    setCapacity(objPtr, kDefaultDictSize);
    setSize(objPtr, 0);
    setBucketCount(objPtr, kDefaultDictSize / 4);
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

Value DictClass::getBucketCount(intptr_t object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kDictBucketCountOffset);
}

void DictClass::setBucketCount(intptr_t object, Value size) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kDictBucketCountOffset) = size;
}

Value DictClass::getBucketBySlot(intptr_t object, Slot slot) {
    char* objPtr = reinterpret_cast<char *>(object);
    return ((Value*)(objPtr + kDictBucketSlotOffset))[slot];
}

void DictClass::setBucketBySlot(intptr_t object, Slot slot, Value value) {
    char* objPtr = reinterpret_cast<char *>(object);
    ((Value*)(objPtr + kDictBucketSlotOffset))[slot] = value;
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
    auto hash = std::hash<Int>{}(key);
    auto capacityValue = capacity(object);
    auto bucketCount = getBucketCount(object);
    auto pos = (hash % bucketCount);

    auto bucket = getBucketBySlot(object, (Slot) pos);

    if(bucket == 0){
        // slot is empty, let's create a bucket
        auto bucketObj = vm->arrayClass->allocate(vm, 4);
        setBucketBySlot(object, pos, bucketObj);
    }

    bucket = getBucketBySlot(object, pos);
    auto countOfItemBucket = vm->arrayClass->getLength(bucket);
    bool founded = false;
    for(auto i = 0; i < countOfItemBucket; i ++ ) {
        auto itemEntryObj = vm->arrayClass->get(bucket, i);
        auto entryKey = vm->dictEntryClass->getKey(itemEntryObj);

        if(entryKey == key) {
            founded = true;
            vm->dictEntryClass->setValue(itemEntryObj, value);
            break;
        }
    }

    if(!founded) {
        // create entry in bucket
        auto entryObj = vm->dictEntryClass->allocate(vm);
        vm->dictEntryClass->setKey(entryObj, key);
        vm->dictEntryClass->setValue(entryObj, value);
        vm->arrayClass->append(bucket,entryObj);
    }

}

Value DictClass::get(intptr_t object, Value key) {
    auto hash = std::hash<Int>{}(key);

    auto spaceSize = getBucketCount(object);
    auto pos = (hash % spaceSize);

    auto bucket = getBucketBySlot(object, (Slot) pos);
    if(bucket != 0) {
        auto countOfItemBucket = vm->arrayClass->getLength(bucket);

        for(auto i = 0; i < countOfItemBucket; i ++ ) {
            auto itemEntryObj = vm->arrayClass->get(bucket, i);
            auto entryKey = vm->dictEntryClass->getKey(itemEntryObj);
            auto entryValue = vm->dictEntryClass->getValue(itemEntryObj);
            if(entryKey == key) {
                return entryValue;
            }
        }

    }
    return 0;
}

//------------------------------------------------
// DictEntry implementation
//------------------------------------------------

DictEntry::DictEntry():
Class(std::string("DictEntry")) {
}

intptr_t DictEntry::allocate(IsolateVM *vm) {
    intptr_t object = vm->gc->allocate(this, kDictEntryValueOffset + kIntSize);
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