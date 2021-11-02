#include "joyeer/runtime/buildin.h"
#include "joyeer/runtime/sys/array.h"
#include "joyeer/runtime/sys/print.h"
#include "joyeer/runtime/sys/string.h"
#include "joyeer/runtime/sys/map.h"
#include "joyeer/runtime/sys/module.h"

std::vector<JrType*> Global::types = {};
std::vector<JrModuleClass*> Global::modules = {};
std::vector<std::string> Global::strings = {};
std::vector<JrFunction*> Global::functions = {};

// init the types tables
static std::unordered_map<std::string, JrFunction*>  funtionsMap;

void Global::initGlobalTables() {

    // Register primary types
    registerObjectType(JrType::Any);
    registerObjectType(JrType::Nil);
    registerObjectType(JrPrimaryType::Int);
    registerObjectType(JrPrimaryType::Float);
    registerObjectType(JrPrimaryType::Boolean);
    
    // Init Module
    JrModule::init();
    // Init print function
    JrFuncPrint::init();
    JrFuncObjectPrint::init();
    // Init the array
    JrObjectArray::init();
    // initialize the string object
    JrObjectString::init();
    // initialize the string builder object
    JrObjectStringBuilder::init();
    // initialize the map object
    JrObjectMap::init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////

/// functions
void Global::registerFunction(JrFunction* func, JrType* ownerType) {
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
