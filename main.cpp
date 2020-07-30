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
    
    NodeDebugPrinter syntaxDebugger(L"asm.parser.txt");
    syntaxDebugger.print(sourceBlock);
    syntaxDebugger.close();
    
    auto compileContext = std::make_shared<CompileContext>();
    
    Binder binder(compileContext);
    binder.bind(sourceBlock);
    
    NodeDebugPrinter binderDebugger(L"asm.binder.txt");
    binderDebugger.print(sourceBlock);
    binderDebugger.close();
    
    TypeChecker typeChecker(compileContext);
    typeChecker.verify(std::static_pointer_cast<Node>(sourceBlock));
    
    NodeDebugPrinter typecheckerDebugger(L"asm.typechecker.txt");
    typecheckerDebugger.print(sourceBlock);
    typecheckerDebugger.close();
    
    IRGen irGen(compileContext);
    irGen.emit(std::static_pointer_cast<Node>(sourceBlock));
    
    TypeTablePrinter typePrinter(L"table.types.txt");
    typePrinter.print();
    typePrinter.close();
    
    FunctionTablePrinter funcPrinter(L"table.functions.txt");
    funcPrinter.print();
    funcPrinter.close();
    
    auto function  = irGen.getFunction();
    
    auto context = std::make_shared<JrRuntimeContext>();
    JrInterpreter interpreter(context);
    interpreter.run(function);
    
    return 0;
}
