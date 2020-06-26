#include "function.h"

JrCode::JrCode(uint32_t size):
size(size),
code((uint8_t*)malloc(size)) {
}

JrCode::~JrCode() {
    free(code);
}


void JrNativeCode::operator()(JrRuntimeContext *context, JrFunction* func) {
}


JrFunction::~JrFunction() {
    switch (kind) {
        case JrFunction_Native:
            delete nativeCode;
            break;
        case JrFunction_VM:
            delete code;
            break;
    }
}
