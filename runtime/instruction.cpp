#include "instruction.h"
#include <iostream>
#include <sstream>

void JrCodeWriter::write(const Instruction& instruction) {
    instructions.push_back(instruction);
}

void JrCodeWriter::write(const std::vector<Instruction> &inss) {
    instructions.insert(instructions.end(), std::begin(inss), std::end(inss));
}

const std::unordered_map<Opcode, std::wstring> JrInstructionDebugPrinter::maps = {
    { OP_NOP, L"nop" },
    { OP_NEW, L"new" },
    { OP_OCONST_NIL, L"const.nil" },
    { OP_ICONST, L"const.int" },
    { OP_ICONST_64, L"const.int.64" },
    { OP_SCONST, L"const.string" },
    
    { OP_ISTORE, L"store.int" },
    { OP_ILOAD, L"load.int" },
    { OP_OLOAD, L"oload" },
    { OP_PUTFIELD, L"putfield"},
    { OP_GETFIELD, L"getfield"},
    
    { OP_IAND, L"and.int" },
    { OP_INEG, L"neg.int" },
    { OP_ICMP_G, L"cmp.large.int" },
    { OP_ICMP_GE, L"cmp.large.equals.int" },
    { OP_ICMP_L, L"cmp.less.int" },
    { OP_ICMP_LE, L"cmp.less.equals.int" },
    { OP_IADD, L"add.int" },
    { OP_ISUB, L"sub.int" },
    { OP_IMUL, L"mul.int" },
    { OP_IDIV, L"div.int" },
    { OP_IREM, L"rem.int" },
    
    { OP_IFEQ, L"if.eq" },
    { OP_IFNE, L"if.ne" },
    { OP_IFLT, L"if.lt" },
    { OP_IFLE, L"if.le" },
    { OP_IFGT, L"if.gt" },
    { OP_IFGE, L"if.ge" },
    
    { OP_RETURN, L"return" },
    { OP_IRETURN, L"return.int" },
    { OP_INVOKE, L"invoke" },
    { OP_GOTO, L"goto"},
    { OP_DEBUG, L"debug"},
    { OP_ONEWARRAY, L"new.array.object"}
};

std::wstring JrInstructionDebugPrinter::print(const Instruction& instruction) {
    std::wstringstream ss;
    ss << JrInstructionDebugPrinter::maps.find(instruction.opcode)->second << L", " <<  instruction.value;
    return ss.str();
}
