//
// Created by Qing Xu on 2021/11/4.
//

#ifndef __joyeer_vm_metadata_h__
#define __joyeer_vm_metadata_h__

struct IsolateVM;
struct JrObject;

#include <vector>
#include <cstdlib>

// Joyeer VM primary types
typedef char            JrByte;
typedef short           JrShort;
typedef int64_t         JrInt;
typedef bool            JrBool;
typedef uintptr_t       JrAddress;
typedef const char*     JrString;
typedef uintptr_t       JrObjectPtr;
typedef uintptr_t       JrFuncPtr;
typedef uintptr_t       JrAny;
typedef uintptr_t       JrBlob;

union JrValue {
    JrInt           intValue;
    JrBool          boolValue;
    JrObjectPtr     objValue;
};

// Constants
constexpr int kJrByteSize = sizeof(JrByte);
constexpr int kJrIntSize = sizeof(JrInt);
constexpr int kJrBoolSize = sizeof (JrBool);
constexpr int kJrValueSize = sizeof(JrValue);
constexpr int kJrObjectSize = sizeof(JrObjectPtr);
constexpr int KJrAnySize = sizeof(JrAny);

// Value Types
enum class JrValueType : uintptr_t {
    Any,
    Int,
    Bool,
    Object,
    Blob,
};


// Class's field description
struct JrField {
#define DECLARE_TYPE(type, size) size,

    constexpr static const size_t ValueTypes[] = {
        DECLARE_TYPE(JrValueType::Any, KJrAnySize)
        DECLARE_TYPE(JrValueType::Int, kJrIntSize)
        DECLARE_TYPE(JrValueType::Bool, kJrBoolSize)
        DECLARE_TYPE(JrValueType::Class, kJrObjectSize)
    };

    JrValueType type;

    explicit JrField(JrValueType type):type(type) {}

    // get the size of the field
    [[nodiscard]] size_t getSize() const {
        return ValueTypes[static_cast<uintptr_t>(type)];
    }

#undef DECLARE_TYPE
};

struct JrArguments {

};

struct JrMethod {
    virtual JrValue operator () (IsolateVM* vm, JrArguments* args) = 0;
};

// Class description
struct JrClass {

    int idx = -1;

    std::vector<JrField> instanceFields {};
    std::vector<JrField> staticFields {};

    std::vector<JrFuncPtr> instanceMethods {};
    std::vector<JrFuncPtr> staticMethods {};

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

struct JrFileModuleClass : public JrClass {
    JrObject* static_ = nullptr;
};

//
struct JrArrayClass : public JrClass {
    constexpr static int kArrayLengthOffset = 0;
    constexpr static int kArrayDataOffset = kArrayLengthOffset + kJrIntSize;

    explicit JrArrayClass(): JrClass() {
    }
};

struct JrArrayClass_$$_size: public JrMethod {

};

//
struct JrIntClass : public JrClass {
    constexpr static int kIntValueOffset = 0;
    explicit JrIntClass(): JrClass() {
        instanceFields.emplace_back(JrValueType::Int);
    }
};



#endif //__joyeer_vm_metadata_h__
