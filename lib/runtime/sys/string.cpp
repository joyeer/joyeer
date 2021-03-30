#include "joyeer/runtime/buildin.h"
#include "joyeer/runtime/sys/string.h"
#include "joyeer/runtime/instruction.h"
#include "joyeer/runtime/gc.h"


auto JrObjectStringInitializer = [](JrPtr memory) -> JrObject* {
    return new(memory) JrObjectString;
};

auto JrObjectStringFinalizer = [](JrObject* object) {
    auto stringObject = (JrObjectString*)object;
    stringObject->~JrObjectString();
};

struct JrObjectStringType : public JrObjectType {
    JrObjectStringType():
        JrObjectType("String", JrObjectStringInitializer, JrObjectStringFinalizer) {
    }
    
    JrInt size() {
        return sizeof(JrObjectString);
    }
};

JrObjectType* JrObjectString::Type = new JrObjectStringType();

JrObjectString::JrObjectString() :
content(new std::string()){
}

JrObjectString::~JrObjectString() {
    delete content;
}

void JrObjectString::init() {
    Global::registerObjectType(JrObjectString::Type);
}

////////////////////////////////////////////////////////////////////////////////////////////////

auto JrObjectStringBuilderInitializer = [](JrPtr memory) -> JrObject* {
    return new (memory) JrObjectStringBuilder;
};

auto JrObjectStringBuilderFinalizer = [](JrObject* object) {
    auto builderObject = (JrObjectStringBuilder*)object;
    builderObject->~JrObjectStringBuilder();
};

struct JrObjectStringBuilderType: public JrObjectType {
    JrObjectStringBuilderType():
        JrObjectType("StringBuilder", JrObjectStringBuilderInitializer, JrObjectStringBuilderFinalizer) {
    }
    
    JrInt size() {
        return sizeof(JrObjectStringBuilder);
    }
};

JrObjectType* JrObjectStringBuilder::Type = new JrObjectStringBuilderType();
JrFunction* JrObjectStringBuilder::Constructor;
JrFunction* JrObjectStringBuilder_Append::Func;
JrFunction* JrObjectStringBuilder_toString::Func;

JrObjectStringBuilder::JrObjectStringBuilder()
:stringstream(new std::stringstream()) {
}

JrObjectStringBuilder::~JrObjectStringBuilder() {
    delete stringstream;
}

void JrObjectStringBuilder::init() {
    Global::registerObjectType(JrObjectStringBuilder::Type);
    
    JrObjectStringBuilder::Constructor = new JrFunction {
        .name = "StringBuilder@StringBuilder()",
        .kind = jrFuncConstructor,
        .paramTypes = { JrObjectStringBuilder::Type },
        .localVars = { JrVar {
            .name = "self",
            .type = JrObjectStringBuilder::Type,
            .addressOfVariable = 0
        }},
        .returnType = JrObjectStringBuilder::Type,
        .instructions = {
                         { .opcode = OP_OLOAD, .value = 0 },
                         { .opcode = OP_ORETURN, .value = 0 }
        }
    };
    
    JrObjectStringBuilder_Append::Func = new JrFunction {
        .name = "StringBuilder@append(content:)",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectStringBuilder::Type, JrObjectString::Type },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectStringBuilder_Append()
    };
    
    JrObjectStringBuilder_toString::Func = new JrFunction {
        .name = "StringBuilder@toString()",
        .kind = jrFuncNative,
        .paramTypes = { JrObjectStringBuilder::Type },
        .returnType = JrPrimaryType::Void,
        .nativeCode = new JrObjectStringBuilder_toString()
    };
    
    Global::registerFunction(JrObjectStringBuilder::Constructor, JrObjectStringBuilder::Type);
    Global::registerFunction(JrObjectStringBuilder_Append::Func, JrObjectStringBuilder::Type);
    Global::registerFunction(JrObjectStringBuilder_toString::Func, JrObjectStringBuilder::Type);
}

void JrObjectStringBuilder_Append::operator()(JrRuntimeContext* context, JrFunction* func) {
    auto stringObjRef = context->stack->pop();
    auto stringObject = (JrObjectString*)context->gc->get(stringObjRef.objRefValue);
    
    auto stringBuildObjRef = context->stack->pop();
    auto stringBuilder = (JrObjectStringBuilder*)context->gc->get(stringBuildObjRef.objRefValue);
    std::stringstream& sstream = *(stringBuilder->stringstream);
    sstream << *(stringObject->content);
}

void JrObjectStringBuilder_toString::operator()(JrRuntimeContext* context, JrFunction* func) {
    auto stringBuildObjRef = context->stack->pop();
    auto stringBuilder = (JrObjectStringBuilder*)context->gc->get(stringBuildObjRef.objRefValue);
    
    auto stringObjRef = context->gc->alloc(JrObjectString::Type);
    auto stringObject = (JrObjectString*)context->gc->get(stringObjRef);
    stringObject->content = new std::string(stringBuilder->stringstream->str());
    
    context->stack->push({.kind = typeString, .objRefValue = stringObjRef});
}
