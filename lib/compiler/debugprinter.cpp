#include "debugprinter.h"
#include "joyeer/compiler/node+stmt.h"
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NodeDebugPrinter::NodeDebugPrinter(const std::string& filename) {
    output.open(filename);
}

void NodeDebugPrinter::printTab() {
    output << std::string(size*2, ' ') ;
}

void NodeDebugPrinter::incTab() {
    size ++;
}

void NodeDebugPrinter::decTab() {
    size --;
}

void NodeDebugPrinter::close() {
    output.flush();
    output.close();
}
