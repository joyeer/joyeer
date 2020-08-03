#include "module.h"

const JrObjectType::Pointer JrModule::Type = JrObjectType::Pointer(new JrObjectType {
    { .kind = JrType_Object, .name = L"Module" }
});
;

void JrModule::init() {
}
