//
// Created by Qing Xu on 2021/11/26.
//

#ifndef __joyeer_runtime_types_h__
#define __joyeer_runtime_types_h__

#include <cstdlib>
#include <string>
#include <vector>

struct Bytecodes;
struct Arguments;
struct IsolateVM;

typedef char            Byte;
typedef int64_t         Int;
typedef uint64_t        UInt;
typedef int32_t         Int32;
typedef int16_t         Int16;
typedef bool            Bool;
typedef uintptr_t       Any;
typedef intptr_t        Slot;
typedef intptr_t        Value;
typedef const char*     FramePtr;

/**
 * ObjectHead for Object's head or Optional Wrapped Value's head
 */
struct ObjectHead {
    bool wrapped:     1 = 0;        // for Optional type 1, else 0
    bool absent:      1 = 0;        // only available for Optional type
    bool reserved:    6 = 0;        // reserved for future usage
    int typeSlot:     20  = -1;
    int refCount:     12 = 0;
    int reserved_2:   24 = 0;       // reserved for future usage
};

struct Object {
    ObjectHead head;
    Byte data[];
};


// Constants
constexpr int kValueSize = sizeof(Value);
constexpr int kByteSize = sizeof(Byte);
constexpr int kIntSize = sizeof(Int);
constexpr int kBoolSize = sizeof (Bool);

// Value Types
enum class ValueType : uint8_t {
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
    Func
};

enum class BuildIns : uint16_t {
    Func_Print = static_cast<size_t>(ValueType::RESOLVED_PRIMARY_TYPE_COUNT) - 1,
    Func_AutoWrapping_Int,
    Func_AutoWrapping_Bool,

    Object_Optional,

    Object_Array,
    Object_Array_Func_size,
    Object_Array_Func_get,
    Object_Array_Func_set,

    Object_Dict,
    Object_Dict_Init,
    Object_Dict_Func_insert,
    Object_Dict_Func_get,

    Object_DictEntry
};


enum AccessFlag : uint32_t {
    Public = 0x0001,
    Private = 0x0002,
    Protected = 0x0004,
    Static = 0x0008
};

std::string debugAccessFlag(AccessFlag flag);

// Type is generated by compiler, which represent a **Type**
struct Type {
    std::string name;
    ValueType kind;
    int slot;

protected:
    Type(std::string  name, ValueType kind):
            name(std::move(name)),
            kind(kind),
            slot(-1) {
    }
};

// Represent Int kind
struct IntType : Type {
    IntType();
};

// Represent Bool kind
struct BoolType : Type {
    BoolType();
};

// represent Nil kind
struct NilType : Type {
    NilType();
};

// represent Void kind
struct VoidType : Type {
    VoidType();
};

// represent Any kind
struct AnyType : Type {
    AnyType(): Type("Any", ValueType::Any) {}
};

// Represent a type are not unspecified, which only available in compiler stage
struct UnspecifiedType: Type {
    UnspecifiedType(): Type("Unspecified", ValueType::Unspecified) {}
};
;

// Variable for Field/LocalVariable declarations
// will be used in Class/ModuleClass/Function
struct Variable {

    // the debugger name's variable/field
    const std::string name;

    int parentSlot = -1; // Variable's parentTypeSlot Type, e.g. Class/ModuleClass/Function
    int typeSlot = -1; // Variable's type
    int loc = -1;

    AccessFlag accessFlags = AccessFlag::Public;

    void markAsStatic() {
        accessFlags = static_cast<AccessFlag>( accessFlags | AccessFlag::Static);
    }

    // is a static variable, field only
    [[nodiscard]] bool isStatic() const {
        return (accessFlags & AccessFlag::Static) == AccessFlag::Static;
    }

    explicit Variable(std::string  name): name(std::move(name)) {}

    int getSize() {
        return kValueSize;
    }
};


enum FuncTypeKind : uint8_t {
    C_Func, // function implemented in C native
    C_CInit, // class constructor implemented in C native
    VM_Func, // function implemented in VM bytecodes
    VM_CInit, // class constructor implemented in VM bytecodes
};

struct Executor;
struct Argument;

typedef Value (*CFunction)(Executor* executor, Argument* argument);

// Represent Function kind (include class's function kind)
struct Function : Type {

    FuncTypeKind funcKind;

    // localVars contains all function params at begin of the array
    int paramCount = 0;
    std::vector<Variable*> localVars = {};
    int returnTypeSlot = -1;
    bool isStatic = false;

    union {
        Bytecodes* bytecodes = nullptr;
        CFunction cFunction;
    };

    explicit Function(const std::string& name, bool isStatic);

    // return the number of total local variable count;
    [[nodiscard]] int getLocalVarCount() const ;

    // get param decl
    Variable* getParamByIndex(int index);
};

/*
 * A type that represents a wrapped value or nil, absence
 *
 * Optional Memory Layout
 * +--------------+
 * | Wrapped Head |
 * +--------------+
 * | wrappedValue |
 * +--------------+
 */

struct Optional : public Type {
    struct DataMap {
        ObjectHead head {};
        Value wrappedValue {};
    };

    explicit Optional();

    intptr_t allocate(IsolateVM* vm, Int value);
    intptr_t allocate(IsolateVM* vm, Bool value);

    // return the wrapped Object's type slot
    Slot valueType(intptr_t objAddr);

    Int intValue(intptr_t objAddr);
    Bool boolValue(intptr_t objAddr);
};


struct Class : public Type {
    constexpr static int kObjectHeadOffset = 0;

    // Class static memory area in GC heap
    intptr_t staticArea = -1;

    // static initializer slot
    Slot staticInitializerSlot = -1;

    IsolateVM* vm = nullptr;

    std::vector<Variable*> staticFields {};
    std::vector<Variable*> instanceFields {};

    explicit Class(const std::string& name);

    virtual intptr_t allocate(IsolateVM* vm);

    // get static fields size
    [[nodiscard]] size_t getStaticSize() const {
        size_t size = 0;
        for(const auto& fields: staticFields) {
            size += fields->getSize();
        }
        return size;
    }
};

struct ModuleClass : public Class {

    explicit ModuleClass(const std::string& name);
};


// Array Object class
struct ArrayClass : public Class {
    constexpr static int kArrayCapacityOffset = kObjectHeadOffset + kValueSize;
    constexpr static int kArrayLengthOffset = kArrayCapacityOffset + kValueSize;
    constexpr static int kArrayDataOffset = kArrayLengthOffset + kValueSize;

    explicit ArrayClass();

    intptr_t allocate(IsolateVM* isolateVm, int capacity);

    void setCapacity(intptr_t object, Value capacity);
    Value getCapacity(intptr_t object);

    void setLength(intptr_t object, Value length);
    Value getLength(intptr_t object);

    void append(intptr_t object, Value value);

    void set(intptr_t object, Value index, Value value);
    Value get(intptr_t object, Value index);

private:
    // calculate array object size based on capacity
    // result size should be (power of 2 + kArrayDataOffset)
    static size_t calculateArrayCapacitySize(int size);
};


/*
 * Dict object Memory alignment
 * +-------------+
 * | Object Head |
 * +-------------+
 * | Dict Size   |
 * +-------------+
 * | Bucket Size |  array of Dict bucket
 * +-------------+
 * | bucket 1.   |
 * | bucket 2.   |
 * | bucket 3.   |
 * | ....        |
 * +-------------+
 *
 *  Dict buckets are an array contains object
 */
struct DictClass : public Class {

    constexpr static int kDictCapacityOffset = kObjectHeadOffset + kValueSize;
    constexpr static int kDictSizeOffset = kDictCapacityOffset + kValueSize;
    constexpr static int kDictBucketCountOffset = kDictSizeOffset + kValueSize;
    constexpr static int kDictBucketSlotOffset = kDictBucketCountOffset + kValueSize;
    constexpr static int kDefaultDictSize = 32; // default size

    explicit DictClass();

    intptr_t allocate(IsolateVM* vm) override;

    Value capacity(intptr_t thisObj);
    void setCapacity(intptr_t thisObj, Value capacity);

    // Dict Bucket count
    Value getBucketCount(intptr_t thisObj);
    void setBucketCount(intptr_t thisObj, Value count);

    // Each space slot contains a dict bucket(Array)
    Value getBucketBySlot(intptr_t thisObj, Slot slot);
    void setBucketBySlot(intptr_t thisObj, Slot slot, Value value);

    Value size(intptr_t thisObj);
    void setSize(intptr_t thisObj, Value value);

    Value resize(intptr_t thisObj);

    void insert(intptr_t thisObj, Value key, Value value);

    Value get(intptr_t thisObj, Value key);
};


struct DictEntry : public Class {
    constexpr static int kDictEntryKeyOffset = kObjectHeadOffset + kIntSize;
    constexpr static int kDictEntryValueOffset = kDictEntryKeyOffset + kIntSize;

    explicit DictEntry();

    intptr_t allocate(IsolateVM* vm) override;

    Value getKey(intptr_t object);
    void setKey(intptr_t object, Value keyValue);

    Value getValue(intptr_t object);
    void setValue(intptr_t object, Value valueValue);

};

#endif //__joyeer_runtime_types_h__
