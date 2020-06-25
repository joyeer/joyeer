#include "buildin.h"
#include "object.h"


struct JrPrintNativeCode : public JrNativeCode {
    void operator()(JrRuntimeContext* context) {
    }
};

const JrFunction BUILDIN_FUNCTIONS[] = {
    {
        .name = L"paint(message:)",
        .kind = JrFunction_Native,
        .paramCount = 1,
        .paramTypes = {{ .kind = JrType_Int, .index = 0}},
        .returnType = { .kind = JrType_Void, .index = 0 },
        .nativeCode = new JrPrintNativeCode()
    }
};

const static std::unordered_map<std::wstring, JrFunction> BUILDIN_FUNCTIONS_MAP;


