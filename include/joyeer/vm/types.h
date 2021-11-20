//
// Created by Qing Xu on 2021/11/4.
//

#ifndef __joyeer_vm_metadata_h__
#define __joyeer_vm_metadata_h__

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


union JrValue {
    JrInt           intValue;
    JrBool          boolValue;
    JrObjectPtr     objValue;
};

// Constants
constexpr int kSystemPointerSize = sizeof(void*);
constexpr int kJrByteSize = sizeof(JrByte);
constexpr int kJrIntSize = sizeof(JrInt);
constexpr int kJrBoolSize = sizeof (JrBool);
constexpr int kJrValueSize = sizeof(JrValue);
constexpr int kJrObjectSize = sizeof(JrObjectPtr);

// Value Types
enum class JrValueType : uintptr_t {
    Int,
    Bool,
    Object,
};


// Class's field description
struct JrField {
#define DECLARE_TYPE(type, size) size,

    constexpr static const size_t ValueTypes[] = {
        DECLARE_TYPE(JrValueType::Int, kJrIntSize)
        DECLARE_TYPE(JrValueType::Bool, kJrBoolSize)
        DECLARE_TYPE(JrValueType::Class, kJrObjectSize)
    };

    JrValueType type;

    // get the size of the field
    [[nodiscard]] size_t getSize() const {
        return ValueTypes[static_cast<uintptr_t>(type)];
    }

#undef DECLARE_TYPE
};


// Class description
struct JrClass {
    std::vector<JrField*> instanceFields {};
    std::vector<JrField*> staticFields {};

    std::vector<JrFuncPtr> instanceMethods {};
    std::vector<JrFuncPtr> staticMethods {};

    [[nodiscard]] size_t getSize() const {
        size_t size = 0;
        for(const auto& field: instanceFields) {
            size += field->getSize();
        }
        return size;
    }

    [[nodiscard]] size_t getStaticSize() const {
        size_t size = 0;
        for(const auto& field: staticFields) {
            size += field->getSize();
        }
        return size;
    }
};

struct JrArrayClass : public JrClass {

};

struct JrIntClass : public JrClass {
    explicit JrIntClass(): JrClass() {
        instanceFields.push_back(new JrField() {

        })
    }
};



#endif //__joyeer_vm_metadata_h__
