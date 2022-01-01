#include "joyeer/runtime/bytecode.h"

Bytecode::Bytecode(int8_t op, int64_t value) {
    format1.op = op;
    format1.value = value;
}

Bytecode::Bytecode(int8_t op, int32_t value1, int32_t value2) {
    format2.op = op;
    format2.value1 = value1;
    format2.value2 = value2;
}
