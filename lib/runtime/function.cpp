#include "joyeer/runtime/function.h"


void JrNativeFunc::operator()(JrRuntimeContext* context, JrFunction* func) {
}


JrFunction::~JrFunction() {
    if((kind & jrFuncNative) == jrFuncNative) {
        delete nativeCode;
    }
}

int JrFunction::totalSizeOfParams() {
    int size = 0;
    for(auto param: paramTypes) {
        size += sizeof(JrValueHold);
    }
    return size;
}
