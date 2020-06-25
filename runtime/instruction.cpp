#include "instruction.h"

void JrCodeWriter::write(const Instruction& instruction) {
    instructions.push_back(instruction);
}
