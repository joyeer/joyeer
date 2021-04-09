#ifndef __joyeer_compiler_node_debugprint_h__
#define __joyeer_compiler_node_debugprint_h__

#include "joyeer/compiler/node.h"

// ASM node printer
struct NodeDebugPrinter {
    NodeDebugPrinter(const std::string filename);
    
    void print(Node::Ptr node);
    void close();
    
protected:
    void print(std::vector<Node::Ptr> nodes);
    void print(SymbolTable::Ptr symtable);
    void print(Symbol::Ptr symbol);
    void printTab();
    void incTab();
    void decTab();
    
private:
    int size = 0;
    std::ofstream output;
};

#endif
