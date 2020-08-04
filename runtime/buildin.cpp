#include "buildin.h"
#include "runtime/sys/array.h"
#include "runtime/sys/print.h"
#include "runtime/sys/module.h"

std::vector<JrType::Pointer> Global::types = {};
std::vector<std::wstring> Global::strings = {};
std::vector<JrFunction::Pointer> Global::functions = {};

// init the types tables
static std::unordered_map<std::wstring, JrFunction::Pointer>  funtionsMap;

void Global::initGlobalTables() {

    // Register primary types
    registerObjectType(JrType::Any);
    registerObjectType(JrPrimaryType::Int);
    registerObjectType(JrPrimaryType::Float);
    registerObjectType(JrPrimaryType::Boolean);
    
    // Init Module
    JrModule::init();
    registerObjectType(JrModule::Type);
    
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

/// functions
void Global::registerFunction(JrFunction::Pointer func) {
    func->addressOfFunc = functions.size();
    functions.push_back(func);
}

// Types
void Global::registerObjectType(JrType::Pointer type) {
    type->addressOfType = types.size();
    types.push_back(type);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Debug printer
//////////////////////////////////////////////////////////////////////////////////////////////////
TypeTablePrinter::TypeTablePrinter(const std::wstring filename) {
    output.open(filename);
}

void TypeTablePrinter::print() {
    int index = 0;
    for(auto type: Global::types) {
        output << L"#" << index << L": " << type->name << std::endl;
        if(type->kind == JrType_Object) {
            auto objectType = std::static_pointer_cast<JrObjectType>(type);
            for(auto field: objectType->virtualFields) {
                print(field);
            }
        }
        
        index ++;
    }
}

void TypeTablePrinter::print(JrFieldType::Pointer field) {
    output << L"    @field: #" << field->addressOfField << L" " << field->name << L", " << field->type->name <<std::endl;
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
