#include "map.h"
#include "runtime/gc.h"
#include "runtime/buildin.h"

size_t JrValueHoldHasher::operator()(const JrValueHold & valueHold) const {
    return valueHold.intValue;
}

auto JrObjectMapInitializer = [](JrPtr memory) -> JrObject* {
    return new(memory) JrObjectMap();
};

auto JrObjectMapFinalizer = [](JrObject* object) {
    auto obj = (JrObjectMap*)object;
    obj->~JrObjectMap();
};

struct JrObjectMapType: public JrObjectType {
    JrObjectMapType():
        JrObjectType(L"Map", JrObjectMapInitializer, JrObjectMapFinalizer) {
    }
    
    JrInt size() {
        return sizeof(JrObjectMap);
    }
};

JrObjectType* JrObjectMap::Type = new JrObjectMapType();
JrFunction::Pointer JrObjectMap::Constructor;
JrFunction::Pointer JrObjectMap_Insert::Func;

JrObjectMap::JrObjectMap() {
}

JrObjectMap::~JrObjectMap() {
}

void JrObjectMap::init() {
    Global::registerObjectType(JrObjectMap::Type);
    
    JrObjectMap::Constructor = JrFunction::Pointer(new JrFunction {
        .name = L"Map@Map()",
        .kind = jrFuncConstructor,
        .paramTypes = { JrObjectMap::Type },
        .localVars = { JrVar {
            .name = L"self",
            .type = JrObjectMap::Type,
            .addressOfVariable = 0
        }},
        .returnType = JrObjectMap::Type,
        .instructions = {
                         { .opcode = OP_OLOAD, .value = 0 },
                         { .opcode = OP_ORETURN, .value = 0 }
        }
    });

    JrObjectMap_Insert::Func = JrFunction::Pointer(new JrFunction{
        .name = L"Map@insert(key:value:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectMap::Type, JrType::Any, JrType::Any },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectMap_Insert(),
    });
    
    Global::registerFunction(JrObjectMap::Constructor, JrObjectMap::Type);
    Global::registerFunction(JrObjectMap_Insert::Func, JrObjectMap::Type);

}

void JrObjectMap_Insert::operator()(JrRuntimeContext::Pointer context, JrFunction::Pointer func) {
    auto valueItemHold = context->stack->pop();
    auto keyItemHold = context->stack->pop();
    auto objectRef = context->stack->pop();
    
    auto mapObject = (JrObjectMap*)context->gc->get(objectRef.objRefValue);
    
    mapObject->maps.insert({
        keyItemHold,
        valueItemHold
    });
}
