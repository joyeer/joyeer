#include <iostream>
#include <vector>

#include "runtime/runtime.h"
#include "compiler/lexparser.h"
#include "compiler/syntaxparser.h"
#include "compiler/binder.h"
#include "compiler/typecheck.h"
#include "compiler/IRGen.h"
#include "compiler/diagnostic.h"

#include "runtime/interpreter.h"
#include "runtime/buildin.h"
#include "compiler/context.h"

int main(int argc, char** argv) {
    
    if(argc < 2) {
        std::cout << "input file";
        return -1;
    }
    
    Global::initGlobalTables();
    
    auto filepath = std::string(argv[1]);
    LexParser parser(filepath);
    parser.parse();

    
    SyntaxParser syntaxParser(parser.tokens);
    SourceBlock::Pointer sourceBlock = syntaxParser.parse();
    
    auto compileContext = std::make_shared<CompileContext>();
    
    Binder binder(compileContext);
    binder.bind(sourceBlock);
    
    NodeDebugPrinter debugPrinter;
    debugPrinter.print(sourceBlock);
    std::wcout << std::endl;
    
    TypeChecker typeChecker(compileContext);
    typeChecker.verify(std::static_pointer_cast<Node>(sourceBlock));
    
    
    
    IRGen irGen(compileContext);
    irGen.emit(std::static_pointer_cast<Node>(sourceBlock));
    
    auto function  = irGen.getFunction();
    
    auto context = std::make_shared<JrRuntimeContext>();
    JrInterpreter interpreter(context);
    interpreter.run(function);
    
    return 0;
}
