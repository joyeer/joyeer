//
// Created by Qing Xu on 2021/11/4.
//
#include "joyeer/vm/types.h"
#include "joyeer/vm/bytecode.h"


VMethod::VMethod(Bytecodes* bytecodes, int paramCount, int localVarCount):
Method(MethodKind::VM_Method),
paramCount(paramCount),
localVarCount(localVarCount),
bytecodes(bytecodes) {
}

VMethod::~VMethod() {
    delete(bytecodes);
}

Value VMethod::operator()(IsolateVM *vm, Arguments *args) const {
    Value result;
    return result;
}

