#include "buildin.h"
#include "object.h"
#include "runtime.h"
#include <iostream>

std::vector<JrFunction::Pointer> initGlobalFunctionTable() ;

const static std::unordered_map<std::wstring, JrFunction::Pointer>  globalFunctionTableMap;
const std::vector<JrFunction::Pointer> Global::funcTable = initGlobalFunctionTable();

struct JrPrintNativeCode : public JrNativeCode {
    void operator()(JrRuntimeContext* context, JrFunction* func) {
        
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
            .paramTypes = {{ .kind = JrType_Int, .index = 0}},
            .returnType = { .kind = JrType_Void, .index = 0 },
            .nativeCode = new JrPrintNativeCode()
        })
    };
    
    return functions;
}
