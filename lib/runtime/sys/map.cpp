#include "joyeer/runtime/sys/map.h"
#include "joyeer/runtime/gc.h"
#include "joyeer/runtime/buildin.h"

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
JrFunction* JrObjectMap::Constructor;
JrFunction* JrObjectMap_Insert::Func;
JrFunction* JrObjectMap_Get::Func;

JrObjectMap::JrObjectMap() {
}

JrObjectMap::~JrObjectMap() {
}

void JrObjectMap::init() {
    Global::registerObjectType(JrObjectMap::Type);
    
    JrObjectMap::Constructor = new JrFunction {
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
    };

    JrObjectMap_Insert::Func = new JrFunction{
        .name = L"Map@insert(key:value:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectMap::Type, JrType::Any, JrType::Any },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectMap_Insert(),
    };
    
    JrObjectMap_Get::Func = new JrFunction{
        .name = L"Map@get(key:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectMap::Type, JrType::Any },
        .returnType = JrType::Any,
        .nativeCode = new JrObjectMap_Get(),
    };
    
    Global::registerFunction(JrObjectMap::Constructor, JrObjectMap::Type);
    Global::registerFunction(JrObjectMap_Insert::Func, JrObjectMap::Type);
    Global::registerFunction(JrObjectMap_Get::Func, JrObjectMap::Type);

}

void JrObjectMap_Insert::operator()(JrRuntimeContext* context, JrFunction* func) {
    auto valueItemHold = context->stack->pop();
    auto keyItemHold = context->stack->pop();
    auto objectRef = context->stack->pop();
    
    auto mapObject = (JrObjectMap*)context->gc->get(objectRef.objRefValue);
    
    auto keyIterator = mapObject->maps.find(keyItemHold);
    if(keyIterator != mapObject->maps.end()) {
        mapObject->maps.erase(keyIterator);
    }
    
    mapObject->maps.insert({
        keyItemHold,
        valueItemHold
    });
}

void JrObjectMap_Get::operator()(JrRuntimeContext* context, JrFunction* func) {
    auto objectRefHold = context->stack->pop();
    auto valuteItemHold = context->stack->pop();
    auto mapObject = (JrObjectMap*)context->gc->get(objectRefHold.objRefValue);
    auto keyIterator = mapObject->maps.find(valuteItemHold);
    if(keyIterator == mapObject->maps.end()) {
        context->stack->push(JrValueHold {
            .kind = typeObject,
            .objRefValue = JrType::Nil->addressOfType
        });
    } else {
        context->stack->push(keyIterator->second);
    }
}
