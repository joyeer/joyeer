#include <iostream>

#include "compiler/lexparser.h"
#include "compiler/syntaxparser.h"
#include "compiler/binder.h"
#include "compiler/IRGen.h"
#include "compiler/diagnostic.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "input file";
        return -1;
    }

    std::string filepath = std::string(argv[1]);
    LexParser parser(filepath);
    parser.parse();

    SyntaxParser syntaxParser(parser.tokens);
    SourceBlock::Pointer sourceBlock = syntaxParser.parse();
    
    auto symFactory = std::make_shared<SymbolFactory>();
    auto typeFactory = std::make_shared<TypeFactory>();
    
    Binder binder(symFactory, typeFactory);
    binder.bind(sourceBlock);
    
    IRGen irGen;
    irGen.emit(std::static_pointer_cast<Node>(sourceBlock));
    
    return 0;
}
