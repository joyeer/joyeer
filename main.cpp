#include <iostream>
#include <vector>

#include "compiler/lexparser.h"
#include "compiler/syntaxparser.h"
#include "compiler/binder.h"
#include "compiler/typecheck.h"
#include "compiler/IRGen.h"
#include "compiler/diagnostic.h"
#include "runtime/runtime.h"
#include "runtime/interpreter.h"
#include "runtime/buildin.h"

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
    
    std::wcout << L"==syntax=="<<std::endl;
    NodeDebugPrinter debugPrinter;
    debugPrinter.print(sourceBlock);
    
    auto symFactory = std::make_shared<SymbolFactory>();
    auto typeFactory = std::make_shared<TypeFactory>();
    
    Binder binder(symFactory, typeFactory);
    binder.bind(sourceBlock);
    
    std::wcout << std::endl << L"====bind==="<<std::endl;
    debugPrinter.print(sourceBlock);
    std::wcout << std::endl << L"====end====" << std::endl;

    TypeChecker typeChecker;
    typeChecker.verify(std::static_pointer_cast<Node>(sourceBlock));
    
    IRGen irGen;
    irGen.emit(std::static_pointer_cast<Node>(sourceBlock));
    
    auto function  = irGen.getFunction();
    
    JrRuntimeContext context;
    JrInterpreter interpreter(&context);
    interpreter.run(function);
    
    return 0;
}
