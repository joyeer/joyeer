//
// Created by Qing Xu on 2021/10/19.
//

#include "joyeer/compiler/opcode.h"
#include <unordered_map>
#include <sstream>

void JrCodeWriter::write(const Instruction& instruction) {
    instructions.push_back(instruction);
}

void JrCodeWriter::write(const std::vector<Instruction> &inss) {
    instructions.insert(instructions.end(), std::begin(inss), std::end(inss));
}

std::string debugPrint(const Instruction& instruction) {
    static std::unordered_map<Opcode, std::string> maps = {
            { OP_NOP, "nop" },
            { OP_NEW, "new" },
            { OP_OCONST_NIL, "const.nil" },
            { OP_ICONST, "const.int" },
            { OP_ICONST_64, "const.int.64" },
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
    auto instStr = maps[instruction.opcode];
    assert(!instStr.empty());
    ss << maps[instruction.opcode] << " " << instruction.value ;
    return ss.str();
}
