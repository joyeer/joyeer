#include "joyeer/compiler/program.h"
#include "joyeer/compiler/lexparser.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/compiler/binder.h"
#include "joyeer/compiler/typecheck.h"
#include "joyeer/compiler/syntaxparser.h"
#include "joyeer/compiler/IRGen.h"
#include "joyeer/runtime/interpreter.h"
#include "joyeer/runtime/buildin.h"
#include <iostream>

#define CHECK_ERROR_CONTINUE \
    if(Diagnostics::errorLevel != none) { \
        return; \
    }

#define CHECK_ERROR_RETURN_NULL \
    if(Diagnostics::errorLevel != none) { \
        return nullptr; \
    }

Program::Program(CompileOpts::Ptr opts):
options(opts) {
}

void Program::run(std::wstring inputfile) {
    auto sourcelife = findSourceFile(inputfile);
    compile(sourcelife);
    
    JrRuntimeContext context;
    JrInterpreter interpreter(&context);
    interpreter.run(sourcelife->moduleClass);
}

SourceFile* Program::findSourceFile(const std::wstring &path, const std::wstring relativeFolder) {
    auto sourcefile = std::filesystem::path(path);
    if(sourcefile.is_relative()) {
        // check the relative folder
        auto target = std::filesystem::path(relativeFolder) / path;
        if(std::filesystem::exists(target)) {
            sourcefile = target;
        }
    }
    
    auto sourcefilePath = sourcefile.wstring();
    if(sourcefiles.find(sourcefilePath) == sourcefiles.end()) {
        auto sourcefile = new SourceFile();
        sourcefile->open(sourcefilePath);
        sourcefiles.insert({sourcefilePath, sourcefile});
    }
    
    return sourcefiles.find(sourcefilePath)->second;
}


void Program::compile(SourceFile *sourcefile) {
    
    auto context= std::make_shared<CompileContext>(options);
    context->sourceFile = sourcefile;
    
    // lex structure analyze
    LexParser lexParser;
    lexParser.parse(sourcefile->content);
    CHECK_ERROR_CONTINUE
    
    // syntax analyze
    SyntaxParser syntaxParser(lexParser.tokens);
    auto block = syntaxParser.parse();
    block->filename = sourcefile->location.wstring();
    debugPrint(block, block->filename + L".parser.debug.txt");
    CHECK_ERROR_CONTINUE
    
    // Detect for type creating
    Binder binder(context);
    binder.importDelegate = std::bind(&Program::tryImport, this, std::placeholders::_1, std::placeholders::_2);
    
    binder.visit(block);
    debugPrint(block, block->filename + L".binder.debug.txt");
    CHECK_ERROR_CONTINUE
    
    // verify the types
    TypeChecker typeChecker(context);
    typeChecker.visit(std::static_pointer_cast<Node>(block));
    debugPrint(block, sourcefile->location.wstring() + L".typechecker.debug.txt");
    CHECK_ERROR_CONTINUE

    // generate IR code
    IRGen irGen(context);
    sourcefile->moduleClass = irGen.emit(block);
    assert(context->exportedSymbols != nullptr);
    sourcefile->exportedSymbolTable = context->exportedSymbols;
    CHECK_ERROR_CONTINUE
    
}

SourceFile* Program::tryImport(CompileContext::Ptr context, const std::wstring &moduleName) {
    auto sourcefile = context->sourceFile;
    auto relativedFolder = sourcefile->location.parent_path().wstring();
    auto importfile = findSourceFile(moduleName, relativedFolder);
    if(importfile == nullptr) {
        Diagnostics::reportError("Error: Module cannot be found");
        return nullptr;
    }
    compile(importfile);
    
    CHECK_ERROR_RETURN_NULL
    return importfile;
}

void Program::debugPrint(Node::Ptr node, const std::wstring &debugFilePath) {
    if(options->vmDebug) {
        NodeDebugPrinter syntaxDebugger(debugFilePath);
        syntaxDebugger.print(node);
        syntaxDebugger.close();
    }
}

void Program::debugPrint(const std::wstring &debugFilePath) {
    if(options->vmDebug) {
        TypeTablePrinter typePrinter(L"debug.table.types.txt");
        typePrinter.print();
        typePrinter.close();
        
        FunctionTablePrinter funcPrinter(L"debug.table.functions.txt");
        funcPrinter.print();
        funcPrinter.close();
    }
}
