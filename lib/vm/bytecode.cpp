//
// Created by Qing Xu on 2021/11/28.
//

#include "joyeer/vm/bytecode.h"

Bytecodes::Bytecodes(Byte *bytes, int length) {
    bytecodes = static_cast<Byte *>(malloc(length));
    memcpy(bytecodes, bytes, length);
}

Bytecodes::~Bytecodes() {
    free((void*)bytecodes);
}

void BytecodeWriter::write(Bytecode bytecode) {
    buffer.write(bytecode);
}

Bytecodes *BytecodeWriter::getBytecodes() const {
    return new Bytecodes(buffer.buffer, buffer.pos);
}
