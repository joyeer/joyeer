#include "compiler/program.h"
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
