#include "buildin.h"
#include "runtime/sys/array.h"
#include "runtime/sys/print.h"

std::vector<JrType::Pointer> initGlobalTypeTable();

std::vector<JrType::Pointer> Global::types = {};
std::vector<std::wstring> Global::strings = {};
std::vector<JrFunction::Pointer> Global::functions = {};

// init the types tables
static std::unordered_map<std::wstring, JrFunction::Pointer>  funtionsMap;

void Global::initGlobalTables() {
    Global::types = initGlobalTypeTable();

    // Init print function
    JrFuncPrint::init();
    registerFunction(JrFuncPrint::Func);
    
    // Init the array
    JrObjectIntArray::init();
    registerObjectType(JrObjectIntArray::Type);
    registerFunction(JrObjectIntArray_Append::Func);
    registerFunction(JrObjectIntArray_Size::Func);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<JrType::Pointer> initGlobalTypeTable() {
    auto types = std::vector<JrType::Pointer> ({
        JrPrimaryType::Int,
        JrPrimaryType::Float,
        JrPrimaryType::Boolean,
        JrType::Any
    });
    
    return types;
}
/// functions

void Global::registerFunction(JrFunction::Pointer func) {
    func->addressOfFunc = functions.size();
    functions.push_back(func);
}

void Global::registerObjectType(JrObjectType::Pointer type) {
    type->addressOfType = types.size();
    types.push_back(type);
}

TypeTablePrinter::TypeTablePrinter(const std::wstring filename) {
    output.open(filename);
}

void TypeTablePrinter::print() {
    int index = 0;
    for(auto type: Global::types) {
        output << L"#" << index << L": " << type->name << std::endl;
        index ++;
    }
}

void TypeTablePrinter::close() {
    output.close();
}

FunctionTablePrinter::FunctionTablePrinter(const std::wstring filename) {
    output.open(filename);
}

void FunctionTablePrinter::print() {
    int index = 0;
    for(auto function: Global::functions) {
        output << L"#" << index << L": " << function->name << std::endl;
        // print the parameters:
        output << L"    .params" << std::endl;
        int paramIndex = 0;
        for(auto param: function->paramTypes) {
            output << L"        #"<< paramIndex << L": " << param->name << std::endl;
            paramIndex ++;
        }
        
        // print the local variables
        output << L"    .localVars" << std::endl;
        int localVarIndex = 0;
        for(auto localVar: function->localVars) {
            output << L"        #"<< localVarIndex << L": " << localVar.name << L", " << localVar.type->name << std::endl;
            localVarIndex ++;
        }
    
        // print the instructions
        output << L"    .opcodes" << std::endl;
        int instrunctionIndex = 0;
        for(auto instruction: function->instructions) {
            JrInstructionDebugPrinter instructionPrinter;
            output << L"        #" << instrunctionIndex << L": " << instructionPrinter.print(instruction) << std::endl;
            instrunctionIndex ++;
        }
        index ++;
        
        
        output << L"------------------------------------------" << std::endl << std::endl;
    }
}

void FunctionTablePrinter::close() {
    output.close();
}
