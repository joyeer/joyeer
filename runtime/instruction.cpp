#include "instruction.h"

void JrCodeWriter::write(const Instruction& instruction) {
    instructions.push_back(instruction);
}

void JrCodeWriter::write(const std::vector<Instruction> &inss) {
    instructions.insert(instructions.end(), std::begin(inss), std::end(inss));
}

