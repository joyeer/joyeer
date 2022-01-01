//
// Created by Qing Xu on 2021/11/26.
//

#ifndef __joyeer_runtime_types_h__
#define __joyeer_runtime_types_h__

#include <cstdlib>

typedef char            Byte;
typedef int64_t         Int;
typedef bool            Bool;
typedef const char*     String;
typedef uintptr_t       FuncPtr;
typedef uintptr_t       Any;
typedef intptr_t        Slot;
typedef const char*     FramePtr;

union Value {
    Int         intValue;
    Bool        boolValue;
    Slot        slotValue;
};

// Constants
constexpr int kValueSize = sizeof(Value);
constexpr int kByteSize = sizeof(Byte);
constexpr int kIntSize = sizeof(Int);
constexpr int kBoolSize = sizeof (Bool);

// Value Types
enum class ValueType : size_t {
    Void = 0x00,
    Int,
    Bool,
    Nil,
    Unspecified,
    Any,
    String,
    RESOLVED_PRIMARY_TYPE_COUNT [[maybe_unused]],
    Module,
    Class,
    Block,
    Func
};

enum class BuildIns : size_t  {
    Func_Print = static_cast<size_t>(ValueType::RESOLVED_PRIMARY_TYPE_COUNT) - 1,

    Object_Array,
    Object_Array_Func_append,
    Object_Array_Func_size,
    Object_Array_Func_get,
    Object_Array_Func_set
};


#endif //__joyeer_runtime_types_h__
