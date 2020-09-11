#include "function.h"

JrCode::JrCode(uint32_t size):
size(size),
code((uint8_t*)malloc(size)) {
}

JrCode::~JrCode() {
    free(code);
}


void JrNativeFunc::operator()(JrRuntimeContext* context, JrFunction* func) {
}


JrFunction::~JrFunction() {
    if((kind & jrFuncNative) == jrFuncNative) {
        delete nativeCode;
    }
    
    if((kind & jrFuncVM) == jrFuncVM) {
        delete code;
    }
}

int JrFunction::totalSizeOfParams() {
    int size = 0;
    for(auto param: paramTypes) {
        size += sizeof(JrValueHold);
    }
    return size;
}
