#include "joyeer/runtime/instruction.h"
#include <iostream>
#include <sstream>

void JrCodeWriter::write(const Instruction& instruction) {
    instructions.push_back(instruction);
}

void JrCodeWriter::write(const std::vector<Instruction> &inss) {
    instructions.insert(instructions.end(), std::begin(inss), std::end(inss));
}

const std::unordered_map<Opcode, std::string> JrInstructionDebugPrinter::maps = {
    { OP_NOP, "nop" },
    { OP_NEW, "new" },
    { OP_OCONST_NIL, "const.nil" },
    { OP_ICONST, "const.int" },
    { OP_ICONST_64, "const.int.64" },
    { OP_SCONST, "const.string" },
    
    { OP_ISTORE, "store.int" },
    { OP_ILOAD, "load.int" },
    { OP_OLOAD, "oload" },
    { OP_PUTFIELD, "putfield"},
    { OP_GETFIELD, "getfield"},
    
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

std::string JrInstructionDebugPrinter::print(const Instruction& instruction) {
    std::stringstream ss;
    ss << JrInstructionDebugPrinter::maps.find(instruction.opcode)->second << ", " <<  instruction.value;
    return ss.str();
}
