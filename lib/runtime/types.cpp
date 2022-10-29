//
// Created by Qing Xu on 2021/11/26.
//
#include "joyeer/runtime/types.h"
#include "joyeer/runtime/isolate+vm.h"

#include <sstream>
#include <functional>
#include <cassert>

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
        funcType(FuncType::VM_Func) {
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

Optional::Optional(const std::string& name, Slot typeSlot):
Type(name, ValueType::Optional) {
    wrappedTypeSlot = (int)typeSlot;
}

AddressPtr Optional::allocate(IsolateVM *vm, Int value) {
    auto objAddr = vm->gc->allocate(this, sizeof(DataMap));
    auto objPtr = reinterpret_cast<DataMap*>(objAddr);
    objPtr->head.wrapped = true;
    objPtr->head.absent = false;
    objPtr->head.typeSlot = (*vm->types)[ValueType::Int]->slot;
    objPtr->wrappedValue = value;

    return objAddr;
}

AddressPtr Optional::allocate(IsolateVM *vm, Bool value) {
    auto objAddr = vm->gc->allocate(this, sizeof(DataMap));
    auto objPtr = reinterpret_cast<DataMap*>(objAddr);
    objPtr->head.wrapped = true;
    objPtr->head.absent = false;
    objPtr->head.typeSlot = (*vm->types)[ValueType::Bool]->slot;
    objPtr->wrappedValue = value;
    return objAddr;
}

AddressPtr Optional::allocateForAddress(IsolateVM *vm, AddressPtr objAddr) {
    auto originalObjectHead = (ObjectHead*)objAddr;
    assert(originalObjectHead->typeSlot != -1);

    auto optionalObjAddr = vm->gc->allocate(this, sizeof(DataMap));
    auto objPtr = reinterpret_cast<DataMap*>(optionalObjAddr);
    objPtr->head.wrapped = true;
    objPtr->head.absent = false;
    objPtr->head.typeSlot = slot;
    objPtr->wrappedValue = objAddr;
    return optionalObjAddr;
}

Slot Optional::valueType(AddressPtr objAddr) {
    auto optionalObj = reinterpret_cast<DataMap*>(objAddr);
    if(optionalObj == nullptr) {
        return (Slot)ValueType::Nil;
    }
    return optionalObj->head.typeSlot;
}

Int Optional::intValue(AddressPtr object) {
    auto optionalObj = reinterpret_cast<DataMap*>(object);
    assert(optionalObj->head.typeSlot == (Int)ValueType::Int);
    return optionalObj->wrappedValue;
}

Bool Optional::boolValue(AddressPtr objAddr) {
    auto optionalObj = reinterpret_cast<DataMap*>(objAddr);
    assert(optionalObj->head.typeSlot == (Int)ValueType::Bool);
    return optionalObj->wrappedValue;
}

Value Optional::value(AddressPtr objAddr) {
    auto optionalObj = reinterpret_cast<DataMap*>(objAddr);
    return optionalObj->wrappedValue;
}

//------------/------------------------------------
// Class implementation
//------------------------------------------------

Value Class::ClassData::getField(int index) {
    return ((Value*)data)[index];
}

void Class::ClassData::putField(int index, Value newValue) {
    ((Value*)data)[index] = newValue;
}

Class::Class(const std::string &name):
    Type(name, ValueType::Class){
}

AddressPtr Class::allocate(IsolateVM *vm) {
    int size = getSize();
    AddressPtr addr = vm->gc->allocate(this, sizeof(ClassData) + size);
    return addr;
}

int Class::getSize() {
    int size = 0;
    for(const auto& field: instanceFields) {
        size += field->getSize();
    }
    return size;
}

Value Class::getField(AddressPtr objAddr, int fieldIndex) {
    auto klassObj = reinterpret_cast<ClassData*>(objAddr);
    assert(fieldIndex < instanceFields.size());
    return klassObj->getField(fieldIndex);
}

void Class::putField(AddressPtr objAddr, int fieldIndex, Value newValue) {
    auto klassObj = reinterpret_cast<ClassData*>(objAddr);
    assert(fieldIndex < instanceFields.size());
    klassObj->putField(fieldIndex, newValue);
}


//------------------------------------------------
// Module implementation
//------------------------------------------------

ModuleUnit::ModuleUnit(const std::string &name) :
        Class(name) {
    kind = ValueType::Module;
}

//------------------------------------------------
// StringClass implementation
//------------------------------------------------

AddressPtr StringClass::allocate(IsolateVM *vm, int typeSlot) {

    auto rawString = (*vm->strings)[typeSlot];
    auto objAddr = vm->gc->allocate(this, (int)(rawString.size() + sizeof(StringData)));
    auto stringObj = reinterpret_cast<StringData*>(objAddr);
    stringObj->head.typeSlot = (Int)ValueType::String;
    stringObj->length = static_cast<Int>(rawString.size());
    memcpy(stringObj->data, rawString.c_str(), stringObj->length);

    return objAddr;
}

AddressPtr StringClass::allocateWithLength(IsolateVM *vm, int size) {
    auto objAddr = vm->gc->allocate(this, (int)(size + kIntSize + sizeof(ObjectHead) + kIntSize));
    auto stringObj = reinterpret_cast<StringData*>(objAddr);
    stringObj->head.typeSlot = (Int)ValueType::String;
    stringObj->length = (Int)size;

    return objAddr;
}

std::string StringClass::toString(AddressPtr objAddr) {
    auto stringObj = reinterpret_cast<StringData*>(objAddr);
    return {stringObj->data, (size_t )stringObj->length };
}

int StringClass::getLength(AddressPtr thisAddr) {
    auto stringObj = reinterpret_cast<StringData*>(thisAddr);
    return stringObj->length;
}

AddressPtr StringClass::c_str(AddressPtr thisAddr) {
    auto stringObj = reinterpret_cast<StringData*>(thisAddr);
    return reinterpret_cast<AddressPtr>(stringObj->data);
}

//------------------------------------------------
// StringBuilderClass implementation
//------------------------------------------------
AddressPtr StringBuilderClass::allocate(IsolateVM *vm) {
    auto sbAddr = vm->gc->allocate(this, sizeof(StringBuilderData));
    auto sbObj = reinterpret_cast<StringBuilderData*>(sbAddr);
    auto arrayObj = vm->arrayClass->allocate(vm, 8);
    sbObj->head.typeSlot = (int)BuildIns::Object_StringBuilder;
    sbObj->stringArray = arrayObj;

    return sbAddr;
}

AddressPtr StringBuilderClass::append(AddressPtr thisAddr, AddressPtr stringAddr) {
    auto thisObj = reinterpret_cast<StringBuilderData*>(thisAddr);;
    vm->arrayClass->append(thisObj->stringArray, stringAddr);
    return thisAddr;
}

AddressPtr StringBuilderClass::toString(AddressPtr thisAddr) {
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


AddressPtr ArrayClass::allocate(IsolateVM* vm, int capacity) {
    Int adjustedCapacity = calculateArrayCapacitySize(capacity) * kValueSize;
    AddressPtr object = vm->gc->allocate(this, sizeof(ArrayData) + adjustedCapacity);

    auto arrayObj = reinterpret_cast<ArrayData*>(object);
    arrayObj->capacity = adjustedCapacity;

    return object;
}

Int ArrayClass::calculateArrayCapacitySize(int size) {
    Int result = 2;
    while(result < size) {
        result *= 2;
    }
    return result;
}

void ArrayClass::setCapacity(AddressPtr object, Value capacity) {
    auto arrayObj = reinterpret_cast<ArrayData*>(object);
    arrayObj->capacity = capacity;
}

Value ArrayClass::getCapacity(AddressPtr object) {
    auto arrayObj = reinterpret_cast<ArrayData*>(object);
    return arrayObj->capacity;
}

void ArrayClass::setLength(AddressPtr object, Value length) {
    auto arrayObj = reinterpret_cast<ArrayData*>(object);
    arrayObj->length = length;
}

Value ArrayClass::getLength(AddressPtr object) {
    auto arrayObj = reinterpret_cast<ArrayData*>(object);
    return arrayObj->length;
}

void ArrayClass::append(AddressPtr object, Value value) {
    auto arrayObj = reinterpret_cast<ArrayData*>(object);
    auto lengthValue = getLength(object);
    arrayObj->data[lengthValue] = value;
    arrayObj->length = lengthValue + 1;
}

Value ArrayClass::get(AddressPtr object, Value index) {
    auto arrayObj = reinterpret_cast<ArrayData*>(object);
    return arrayObj->data[index];
}

void ArrayClass::set(AddressPtr object, Value index, Value value) {
    auto arrayObj = reinterpret_cast<ArrayData*>(object);
    arrayObj->data[index] = value;
}

//------------------------------------------------
// DictClass implementation
//------------------------------------------------

DictClass::DictClass():
Class(std::string("Dict")) {

}

AddressPtr DictClass::allocate(IsolateVM *vm) {
    this->vm = vm;
    int bucketCount = kDefaultCapacitySize / kDefaultPerBucketItem ;
    auto objPtr= vm->gc->allocate(this, sizeof(DictData) + bucketCount * kValueSize ) ;
    auto dictObj = reinterpret_cast<DictData*>(objPtr);
    dictObj->capacity= kDefaultCapacitySize;
    dictObj->bucketSize = bucketCount;
    dictObj->count = 0;
    return objPtr;
}

Value DictClass::capacity(AddressPtr object) {
    auto dictObj = reinterpret_cast<DictData*>(object);
    return dictObj->capacity;
}

void DictClass::setCapacity(AddressPtr object, Value capacity) {
    auto dictObj = reinterpret_cast<DictData*>(object);
    dictObj->capacity = capacity;
}

Value DictClass::getBucketCount(AddressPtr object) {
    auto dictObj = reinterpret_cast<DictData*>(object);
    return dictObj->bucketSize;
}

void DictClass::setBucketCount(AddressPtr object, Value size) {
    auto dictObj = reinterpret_cast<DictData*>(object);
    dictObj->bucketSize = size;
}

Value DictClass::getBucketBySlot(AddressPtr object, Slot slot) {
    auto dictObj = reinterpret_cast<DictData*>(object);
    return dictObj->buckets[slot];
}

void DictClass::setBucketBySlot(AddressPtr object, Slot slot, Value value) {
    auto dictObj = reinterpret_cast<DictData*>(object);
    dictObj->buckets[slot] = value;
}

Value DictClass::size(AddressPtr object) {
    auto dictObj = reinterpret_cast<DictData*>(object);
    return dictObj->count;
}

void DictClass::setSize(AddressPtr object, Value value) {
    auto dictObj = reinterpret_cast<DictData*>(object);
    dictObj->count = value;
}

void DictClass::insert(AddressPtr object, Value key, Value value) {
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

Value DictClass::get(AddressPtr object, Value key) {
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
    return NilType::nilValue();
}

//------------------------------------------------
// DictEntry implementation
//------------------------------------------------

DictEntry::DictEntry():
Class(std::string("DictEntry")) {
}

AddressPtr DictEntry::allocate(IsolateVM *vm) {
    AddressPtr object = vm->gc->allocate(this, sizeof(DictEntryData));
    return object;
}

Value DictEntry::getKey(AddressPtr object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kDictEntryKeyOffset);
}

void DictEntry::setKey(AddressPtr object, Value keyValue) {
    char* objPtr = reinterpret_cast<char *>(object);
    *(Value*)(objPtr + kDictEntryKeyOffset) = keyValue;
}

Value DictEntry::getValue(AddressPtr object) {
    char* objPtr = reinterpret_cast<char *>(object);
    return *(Value*)(objPtr + kDictEntryValueOffset);
}

void DictEntry::setValue(AddressPtr object, Value valueValue) {
    char* objPtr = reinterpret_cast<char *>(object);
    auto type = vm->getType((Slot)ValueType::Int);
    auto optionalIntType = (Optional*)vm->getType(type->optionalTypeSlot);
    auto entryValue = optionalIntType->allocateForAddress(vm, valueValue);
    *(Value*)(objPtr + kDictEntryValueOffset) = valueValue;
}