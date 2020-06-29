#include <iostream>

#include "compiler/lexparser.h"
#include "compiler/syntaxparser.h"
#include "compiler/binder.h"
#include "compiler/typecheck.h"
#include "compiler/IRGen.h"
#include "compiler/diagnostic.h"
#include "runtime/runtime.h"
#include "runtime/interpreter.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "input file";
        return -1;
    }

    auto filepath = std::string(argv[1]);
    LexParser parser(filepath);
    parser.parse();

    SyntaxParser syntaxParser(parser.tokens);
    SourceBlock::Pointer sourceBlock = syntaxParser.parse();
    
    auto symFactory = std::make_shared<SymbolFactory>();
    auto typeFactory = std::make_shared<TypeFactory>();
    
    Binder binder(symFactory, typeFactory);
    binder.bind(sourceBlock);
    
    TypeChecker typeChecker;
    typeChecker.verify(std::static_pointer_cast<Node>(sourceBlock));
    
    IRGen irGen;
    irGen.emit(std::static_pointer_cast<Node>(sourceBlock));
    
    std::vector<Instruction> instructions = irGen.getInstructions();
    
    JrRuntimeContext context;
    JrInterpreter interpreter(&context);
    interpreter.run(instructions);
    
    return 0;
}
