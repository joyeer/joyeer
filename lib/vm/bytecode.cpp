//
// Created by Qing Xu on 2021/11/28.
//

#include "joyeer/vm/bytecode.h"

void BytecodeWriter::write(Bytecode bytecode) {
    buffer.write(bytecode);
}
