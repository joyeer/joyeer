#include <iostream>

#include "compiler/program.h"
#include "compiler/lexparser.h"
#include "compiler/syntaxparser.h"
#include "compiler/binder.h"
#include "compiler/typecheck.h"
#include "compiler/IRGen.h"
#include "compiler/diagnostic.h"
#include "runtime/interpreter.h"
#include "runtime/buildin.h"

int main(int argc, char** argv) {
    
    // command line arguments
    auto options = std::make_shared<CompileOpts>(argc, argv);
    
    if(!options->accepted) {
        options->printUsage();
        return -1;
    }
    
    
    Global::initGlobalTables();
    
    Program program(options);
    program.run(options->inputfile.wstring());
//    
//    auto filepath = options->inputfile;
//    
//    LexParser parser(filepath.wstring());
//    parser.parse();
//    
//    SyntaxParser syntaxParser(parser.tokens);
//    SourceBlock::Ptr sourceBlock = syntaxParser.parse();
//    sourceBlock->filename = std::filesystem::path(filepath).filename().wstring();
//    
//    NodeDebugPrinter syntaxDebugger(L"debug.asm.parser.txt");
//    syntaxDebugger.print(sourceBlock);
//    syntaxDebugger.close();
//    
//    auto compileContext = std::make_shared<CompileContext>();
//    
//    Binder binder(compileContext);
//    sourceBlock = binder.bind(sourceBlock);
//    
//    NodeDebugPrinter binderDebugger(L"debug.asm.binder.txt");
//    binderDebugger.print(sourceBlock);
//    binderDebugger.close();
//    
//    TypeChecker typeChecker(compileContext);
//    typeChecker.verify(std::static_pointer_cast<Node>(sourceBlock));
//    
//    NodeDebugPrinter typecheckerDebugger(L"debug.asm.typechecker.txt");
//    typecheckerDebugger.print(sourceBlock);
//    typecheckerDebugger.close();
//    
//    IRGen irGen(compileContext);
//    auto m = irGen.emit(sourceBlock);
//    
//    TypeTablePrinter typePrinter(L"debug.table.types.txt");
//    typePrinter.print();
//    typePrinter.close();
//    
//    FunctionTablePrinter funcPrinter(L"debug.table.functions.txt");
//    funcPrinter.print();
//    funcPrinter.close();
//    
//    auto context = new JrRuntimeContext();
//    JrInterpreter interpreter(context);
//    interpreter.run(m);
    
    return 0;
}
