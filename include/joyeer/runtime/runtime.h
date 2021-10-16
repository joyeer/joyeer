#ifndef __joyeer_runtime_runtime_h__
#define __joyeer_runtime_runtime_h__

#include <stack>
#include <string>
#include "object.h"
#include "instruction.h"


typedef std::function<JrObject*(JrPtr)> JrObjectInitializer;
typedef std::function<void(JrObject*)> JrObjectFinalizer;

struct JrType {
    static JrType* Any;
    static JrType* Void;
    static JrType* Nil;
    
    JrTypeKind kind;
    std::string name;
    
    // index to the type tables
    int addressOfType;
    
    JrType(JrTypeKind kind, const std::string& name);
    
    virtual JrInt size();
    
    // initialize the object after malloc memory data
    JrObjectInitializer initializer = nullptr;
    // finalize the object before release memory
    JrObjectFinalizer finalizer = nullptr;
};

struct JrFieldType {
    typedef std::shared_ptr<JrFieldType> Ptr;
    
    std::string name;
    JrType* type;
    JrInt addressOfField = -1;
};

struct JrPrimaryType: public JrType {
    
    static JrPrimaryType* Int;
    static JrPrimaryType* Float;
    static JrPrimaryType* Boolean;
    
    JrPrimaryType(JrTypeKind kind, const std::string& name);
    
    virtual JrInt size();
};
 
struct JrObjectType: public JrType {
    
    std::vector<JrInt> constructors;
    std::vector<JrInt> virtualFunctions;
    std::vector<JrInt> staticFunctions;
    
    std::vector<JrFieldType::Ptr> virtualFields;
    std::vector<JrFieldType::Ptr> staticFields;
    
    void registerField(JrFieldType::Ptr field);
    
    virtual JrInt size();
    
    JrObjectType(const std::string& name,
                 JrObjectInitializer initializer = nullptr,
                 JrObjectFinalizer finalizer = nullptr);
};

// Speical type for module class
struct JrModuleClass: public JrObjectType {
    JrModuleClass(const std::string& name);
    
    JrInt addressOfMudule;
};

struct JrVar {
    
    JrType* type;
    std::string name;
    
    int addressOfVariable;
    int addressOfObject;
};

struct JrFunctionFrame {
    
    typedef std::shared_ptr<JrFunctionFrame> Ptr;
    
    // the index of the function in function tables
    JrInt addressOfFunc;
    
    // the address of this frame in stack
    JrInt startAddress;
    JrInt endAddress;
    
    // the addresses of variable in stack
    std::vector<JrInt> addressOfVariables;
};


// Runtime stack for VM
struct JrRuntimeStack {
    const static JrInt Size = 1024 * 1024 * 4;
    
    JrRuntimeStack();
    
    std::stack<JrFunctionFrame::Ptr> frames;
    JrInt pointer;
    JrByte data[JrRuntimeStack::Size];
    
    // push the FunctionFrame into stack
    void startFuncCall(JrFunctionFrame::Ptr frame);
    void endFuncCall(JrFunctionFrame::Ptr frame);
    
    JrFunctionFrame::Ptr topFrame();
    
    JrValueHold top();
    
    void push(JrValueHold value);
    JrValueHold pop();
    
    // store the variable value at address at stack
    void storeValueForVariable(JrInt addressOfVariable, JrValueHold value);
    // get the int value of the variable
    JrValueHold intValueOfVariable(JrInt addressOfVariable);
};

struct JrGC;
// The runtime context for VM
struct JrRuntimeContext {
    
    JrRuntimeContext();
    ~JrRuntimeContext();
    
    JrRuntimeStack* stack;
    JrGC* gc;
};

#endif
