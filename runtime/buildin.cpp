#include "buildin.h"
#include "object.h"
#include "runtime.h"
#include <iostream>

std::vector<JrFunction::Pointer> initGlobalFunctionTable();
std::vector<JrType::Pointer> initGlobalTypeTable();


void Global::initGlobalTables() {
    Global::types = initGlobalTypeTable();
    Global::functions =initGlobalFunctionTable();

}

std::vector<JrType::Pointer> Global::types = {};
std::vector<std::wstring> Global::strings = {};
std::vector<JrFunction::Pointer> Global::functions = {};

//////////////////////////////////////////////////////////////////////////////////////////////////
// init the types tables

std::vector<JrType::Pointer> initGlobalTypeTable() {
    auto types = std::vector<JrType::Pointer> ({
        JrPrimaryType::Int,
        JrPrimaryType::Float,
        JrPrimaryType::Boolean,
        JrType::Any
    });
    
    return types;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// init the function tables
static std::unordered_map<std::wstring, JrFunction::Pointer>  funtionsMap;

/// functions

void Global::registerFunction(JrFunction::Pointer func) {
    func->addressOfFunc = functions.size();
    functions.push_back(func);
}

void Global::registerObjectType(JrObjectType::Pointer type) {
    type->addressOfType = types.size();
    types.push_back(type);
}

struct JrPrintNativeCode : public JrNativeCode {
    virtual void operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
        uint32_t value = context->stack->pop4();
        std::cout << value << std::endl;
    }
};

std::vector<JrFunction::Pointer> initGlobalFunctionTable() {
    std::vector<JrFunction::Pointer> functions = {
        std::shared_ptr<JrFunction>(new JrFunction {
            .name = L"print(message:)",
            .kind = JrFunction_Native,
            .paramTypes = { JrType::Any },
            .nativeCode = new JrPrintNativeCode(),
            .addressOfFunc = 0
        })
    };
    
    return functions;
}


