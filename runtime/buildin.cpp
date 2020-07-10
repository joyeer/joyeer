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
            .paramCount = 1,
//            .paramTypes = {{ .name = L"Any", .kind = JrType_Object, .index = 0}},
//            .returnType = { .name = L"Void", .kind = JrType_Void, .index = 0 },
            .nativeCode = new JrPrintNativeCode(),
            .addressOfFunc = 0
        })
    };
    
    return functions;
}


