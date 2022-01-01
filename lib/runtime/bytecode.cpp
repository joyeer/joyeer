#include "joyeer/runtime/bytecode.h"
#include <memory>

Bytecode::Bytecode(int8_t op, int64_t value) {
    format1.op = op;
    format1.value = value;
}

Bytecode::Bytecode(int8_t op, int32_t value1, int32_t value2) {
    format2.op = op;
    format2.value1 = value1;
    format2.value2 = value2;
}

Bytecodes::Bytecodes(Byte *bytes, int length) {
    bytecodes = static_cast<Byte *>(malloc(length));
    memcpy(bytecodes, bytes, length);
}

Bytecodes::~Bytecodes() {
    free((void*)bytecodes);
}

void BytecodeWriter::write(Bytecode bytecode) {
    buffer.write(*(int64_t*)&bytecode);
}

Bytecodes *BytecodeWriter::getBytecodes() const {
    return new Bytecodes(buffer.buffer, buffer.pos);
}
