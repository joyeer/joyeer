//
// Created by Qing Xu on 2021/11/26.
//

#ifndef __joyeer_common_types_h__
#define __joyeer_common_types_h__

#include <cstdlib>

typedef char            Byte;
typedef int64_t         Int;
typedef bool            Bool;
typedef uintptr_t       ObjPtr;
typedef const char*     String;
typedef uintptr_t       FuncPtr;
typedef uintptr_t       Any;
typedef uintptr_t       Blob;

union Value {
    Int           intValue;
    Bool          boolValue;
};

// Constants
constexpr int kValueSize = sizeof(Value);
constexpr int kByteSize = sizeof(Byte);
constexpr int kIntSize = sizeof(Int);
constexpr int kBoolSize = sizeof (Bool);
constexpr int KAnySize = sizeof(Any);

// Value Types
enum class ValueType : uintptr_t {
    Void = 0x00,
    Int,
    Bool,
    Nil,
    Any,
    Module,
    Class,
    Block,
    Function,
    Variable,
    Object,
    Blob,
};

#endif //__joyeer_common_types_h__
