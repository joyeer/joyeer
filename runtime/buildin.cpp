#include "buildin.h"
#include "runtime/sys/array.h"
#include "runtime/sys/print.h"
#include "runtime/sys/string.h"
#include "runtime/sys/map.h"
#include "runtime/sys/module.h"

std::vector<JrType*> Global::types = {};
std::vector<std::wstring> Global::strings = {};
std::vector<JrFunction::Pointer> Global::functions = {};

// init the types tables
static std::unordered_map<std::wstring, JrFunction::Pointer>  funtionsMap;

void Global::initGlobalTables() {

    // Register primary types
    registerObjectType((JrType*)JrType::Any);
    registerObjectType((JrType*)JrPrimaryType::Int);
    registerObjectType((JrType*)JrPrimaryType::Float);
    registerObjectType((JrType*)JrPrimaryType::Boolean);
    
    // Init Module
    JrModule::init();
    // Init print function
    JrFuncPrint::init();
    JrFuncObjectPrint::init();
    // Init the array
    JrObjectIntArray::init();
    // initialize the string object
    JrObjectString::init();
    // initialize the string builder object
    JrObjectStringBuilder::init();
    // initialize the map object
    JrObjectMap::init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

/// functions
void Global::registerFunction(JrFunction::Pointer func, JrType* ownerType) {
    func->addressOfFunc = functions.size();
    if(ownerType != nullptr) {
        func->addressOfOwnerType = ownerType->addressOfType;
    } else {
        func->addressOfOwnerType = -1;
    }
    
    functions.push_back(func);
}

// Types
void Global::registerObjectType(JrType* type) {
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
        if(type->kind == typeObject) {
            auto objectType = (JrObjectType*)type;
            for(auto field: objectType->virtualFields) {
                print(field);
            }
            
            for(auto addressOfFunc: objectType->constructors) {
                print(addressOfFunc);
            }
            for(auto addressOfFunc: objectType->virtualFunctions) {
                print(addressOfFunc);
            }
        }
        
        index ++;
    }
}

void TypeTablePrinter::print(JrFieldType::Pointer field) {
    output << L"    @field: #" << field->addressOfField << L" " << field->name << L", " << field->type->name <<std::endl;
}

void TypeTablePrinter::print(int addressOfFunc) {
    auto func = Global::functions[addressOfFunc];
    auto name = func->kind == jrFuncConstructor ? L"constructor" : L"method";
    output << L"    @"<< name <<": #" << func->addressOfFunc << L" " << func->name <<std::endl;
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
