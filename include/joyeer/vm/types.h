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
typedef char*           JrObject;
typedef uintptr_t       JrAddress;
typedef const char*     JrString;

union JrValue {
    JrInt       intValue;
    JrBool      boolValue;
    JrObject    objValue;
};

// Constants
constexpr int kSystemPointerSize = sizeof(void*);
constexpr int kJrByteSize = sizeof(JrByte);
constexpr int kJrIntSize = sizeof(JrInt);
constexpr int kJrBoolSize = sizeof (JrBool);
constexpr int kJrValueSize = sizeof(JrValue);

// Value Types
enum class JrValueType : uint8_t {
    Int,
    Bool,
    Object,
};

struct JrField {
    JrValueType type;
};

struct JrClass {
    std::vector<JrInt> fields;
    std::vector<JrInt> methods;
};


struct JrFunc {

};
#endif //__joyeer_vm_metadata_h__
