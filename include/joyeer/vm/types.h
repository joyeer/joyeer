//
// Created by Qing Xu on 2021/11/4.
//

#ifndef __joyeer_vm_metadata_h__
#define __joyeer_vm_metadata_h__

struct IsolateVM;
struct Object;

#include <vector>
#include <cstdlib>

// Joyeer VM primary types
typedef char            Byte;
typedef short           Short;
typedef int64_t         Int;
typedef bool            Bool;
typedef uintptr_t       Address;
typedef const char*     String;
typedef uintptr_t       ObjectPtr;
typedef uintptr_t       FuncPtr;
typedef uintptr_t       Any;
typedef uintptr_t       Blob;

union Value {
    Int           intValue;
    Bool          boolValue;
    ObjectPtr     objValue;
};

// Constants
constexpr int kValueSize = sizeof(Value);
constexpr int kByteSize = sizeof(Byte);
constexpr int kIntSize = sizeof(Int);
constexpr int kBoolSize = sizeof (Bool);
constexpr int kObjectSize = sizeof(ObjectPtr);
constexpr int KAnySize = sizeof(Any);

// Value Types
enum class ValueType : uintptr_t {
    Any,
    Int,
    Bool,
    Object,
    Blob,
};


// Class's field description
struct Field {
#define DECLARE_TYPE(type, size) size,

    constexpr static const size_t ValueTypes[] = {
        DECLARE_TYPE(ValueType::Any, KAnySize)
        DECLARE_TYPE(ValueType::Int, kIntSize)
        DECLARE_TYPE(ValueType::Bool, kBoolSize)
        DECLARE_TYPE(ValueType::Class, kObjectSize)
    };

    ValueType type;

    explicit Field(ValueType type): type(type) {}

    // get the size of the field
    [[nodiscard]] size_t getSize() const {
        return ValueTypes[static_cast<uintptr_t>(type)];
    }

#undef DECLARE_TYPE
};

struct Arguments {

};

struct Method {
    virtual Value operator () (IsolateVM* vm, Arguments* args) = 0;
};

// Class description
struct Class {

    int idx = -1;

    std::vector<Field> instanceFields {};
    std::vector<Field> staticFields {};

    std::vector<FuncPtr> instanceMethods {};
    std::vector<FuncPtr> staticMethods {};

    [[nodiscard]] size_t getSize() const {
        size_t size = 0;
        for(const auto& field: instanceFields) {
            size += field.getSize();
        }
        return size;
    }

    [[nodiscard]] size_t getStaticSize() const {
        size_t size = 0;
        for(const auto& field: staticFields) {
            size += field.getSize();
        }
        return size;
    }
};

struct ModuleClass : public Class {

};

//
struct ArrayClass : public Class {
    constexpr static int kArrayLengthOffset = 0;
    constexpr static int kArrayDataOffset = kArrayLengthOffset + kIntSize;

    explicit ArrayClass(): Class() {
    }
};

struct ArrayClass_$$_size: public Method {

};

//
struct IntClass : public Class {
    constexpr static int kIntValueOffset = 0;
    explicit IntClass(): Class() {
        instanceFields.emplace_back(ValueType::Int);
    }
};



#endif //__joyeer_vm_metadata_h__
