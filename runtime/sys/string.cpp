#include "string.h"
#include "runtime/buildin.h"

auto JrObjectStringInitializer = [](JrPtr memory) -> JrObject* {
    return new(memory) JrObjectString;
};

auto JrObjectStringFinalizer = [](JrObject* object) {
    auto arrayObject = (JrObjectString*)object;
    arrayObject->~JrObjectString();
};

struct JrObjectStringType : public JrObjectType {
    JrObjectStringType():
        JrObjectType(L"String", JrObjectStringInitializer, JrObjectStringFinalizer) {
    }
    
    JrInt size() {
        return sizeof(JrObjectString);
    }
};

JrObjectType* JrObjectString::Type = new JrObjectStringType();

JrObjectString::JrObjectString() {
    
}

JrObjectString::~JrObjectString() {
}

void JrObjectString::init() {
    Global::registerObjectType(JrObjectString::Type);
}
