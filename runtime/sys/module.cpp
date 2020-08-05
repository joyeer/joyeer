#include "module.h"

const JrObjectType* JrModule::Type = new JrObjectType {
    { .kind = JrType_Object, .name = L"Module" }
};

void JrModule::init() {
}
