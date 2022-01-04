#include "joyeer/runtime/bytecode.h"
#include <memory>
#include <unordered_map>
#include <sstream>



//-------------------------------------------------------
// Bytecode
//-------------------------------------------------------


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
    size = length / kValueSize;
}

Bytecodes::~Bytecodes() {
    free((void*)bytecodes);
}


//-------------------------------------------------------
// BytecodeWriter
//-------------------------------------------------------

void BytecodeWriter::write(Bytecode bytecode) {
    buffer.write((Int)*((Int*)(&bytecode)));
}

void BytecodeWriter::write(Bytecodes *bytecodes) {
    for(auto i = 0; i < bytecodes->size; i ++ ) {
        auto bytecode = bytecodes[i];
        buffer.write(*(int64_t*)&bytecode);
    }

    free(bytecodes);
}

Bytecodes *BytecodeWriter::getBytecodes() const {
    return new Bytecodes(buffer.buffer, buffer.pos);
}

std::string debugPrint(Bytecode* bytecode) {
    static std::unordered_map<Opcode, std::string> maps = {
            { OP_NOP, "nop" },
            { OP_NEW, "new" },
            { OP_OCONST_NIL, "const.nil" },
            { OP_ICONST, "const.int" },
            { OP_SCONST, "const.string" },

            { OP_ISTORE, "store.int" },
            { OP_ILOAD, "load.int" },
            { OP_OLOAD, "oload" },
            { OP_PUTFIELD, "put.field"},
            { OP_GETFIELD, "get.field"},
            { OP_PUTSTATIC, "put.static"},
            { OP_GETSTATIC, "get.static"},

            { OP_IAND, "and.int" },
            { OP_INEG, "neg.int" },
            { OP_ICMP_G, "cmp.large.int" },
            { OP_ICMP_GE, "cmp.large.equals.int" },
            { OP_ICMP_L, "cmp.less.int" },
            { OP_ICMP_LE, "cmp.less.equals.int" },
            { OP_ICMP_NE, "cmp.not.equals.int"},
            { OP_ICMP_EQ, "cmp.equals.int" },
            { OP_IADD, "add.int" },
            { OP_ISUB, "sub.int" },
            { OP_IMUL, "mul.int" },
            { OP_IDIV, "div.int" },
            { OP_IREM, "rem.int" },

            { OP_IFEQ, "if.eq" },
            { OP_IFNE, "if.ne" },
            { OP_IFLT, "if.lt" },
            { OP_IFLE, "if.le" },
            { OP_IFGT, "if.gt" },
            { OP_IFGE, "if.ge" },

            { OP_RETURN, "return" },
            { OP_IRETURN, "return.int" },
            { OP_ORETURN, "return.object" },
            { OP_INVOKE, "invoke" },
            { OP_GOTO, "goto"},
            { OP_DEBUG, "debug"},
            { OP_ONEWARRAY, "new.array.object"},
            { OP_DUP, "dup" }
    };


    std::stringstream ss;
    auto instStr = maps[(Opcode)bytecode->format1.op];
    assert(!instStr.empty());
    ss << maps[(Opcode)bytecode->format1.op] << " " << bytecode->format1.value ;
    return ss.str();
}
