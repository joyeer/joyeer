#include "joyeer/compiler/compiler+service.h"
#include "debugprinter.h"
#include "joyeer/compiler/lexparser.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/compiler/binder.h"
#include "joyeer/compiler/typecheck.h"
#include "joyeer/compiler/syntaxparser.h"
#include "joyeer/compiler/IRGen.h"
#include "joyeer/runtime/interpreter.h"
#include "joyeer/runtime/buildin.h"
#include <utility>

#define CHECK_ERROR_CONTINUE \
    if(Diagnostics::errorLevel != none) { \
        return; \
    }

#define CHECK_ERROR_RETURN_NULL \
    if(Diagnostics::errorLevel != none) { \
        return nullptr; \
    }

CompilerService::CompilerService(CommandLineArguments::Ptr opts):
options(std::move(opts)) {
    initializeGlobalSymbolTable();
}

void CompilerService::run(const std::string& inputFile) {
    auto sourcefile = findSourceFile(inputFile);
    compile(sourcefile);
    
    JrRuntimeContext context;
    JrInterpreter interpreter(&context);
    interpreter.run(sourcefile->moduleClass);
}

SourceFile::Ptr CompilerService::findSourceFile(const std::string &path, const std::string& relativeFolder) {
    auto sourcefile = std::filesystem::path(path);
    if(sourcefile.is_relative()) {
        // check the relative folder
        auto target = std::filesystem::path(relativeFolder) / path;
        if(std::filesystem::exists(target)) {
            sourcefile = target;
        }
    }
    
    auto sourcefilePath = sourcefile.string();
    if(sourceFiles.find(sourcefilePath) == sourceFiles.end()) {
        auto sf = std::make_shared<SourceFile>(options->workingDirectory, sourcefilePath);
        sourceFiles.insert({sourcefilePath, sf});
    }
    
    return sourceFiles.find(sourcefilePath)->second;
}


void CompilerService::compile(const SourceFile::Ptr& sourcefile) {
    
    auto context= std::make_shared<CompileContext>(options);
    context->sourcefile = sourcefile;
    context->compiler = this;
    context->entry(globalSymbols);
    
    // lex structure analyze
    LexParser lexParser;
    lexParser.parse(sourcefile);
    CHECK_ERROR_CONTINUE
    
    // syntax analyze
    SyntaxParser syntaxParser(sourcefile);
    auto block = syntaxParser.parse();
    block->filename = sourcefile->getAbstractLocation();
    debugPrint(block, block->filename + ".parser.debug.txt");
    CHECK_ERROR_CONTINUE
    
    // Detect for type creating
    Binder binder(context);
    binder.importDelegate = std::bind(&CompilerService::tryImport, this, std::placeholders::_1, std::placeholders::_2);
    
    binder.visit(block);
    debugPrint(block, block->filename + ".binder.debug.txt");
    CHECK_ERROR_CONTINUE
    
    // verify the types
    TypeChecker typeChecker(context);
    typeChecker.visit(std::static_pointer_cast<Node>(block));
    debugPrint(block, sourcefile->getAbstractLocation() + ".typechecker.debug.txt");
    CHECK_ERROR_CONTINUE

    // generate IR code
    IRGen irGen(context);
    sourcefile->moduleClass = irGen.emit(block);
    CHECK_ERROR_CONTINUE
    
    // delcare the FileModuleNode in repos
    declare(block);
    context->leave(globalSymbols);
}

void CompilerService::declare(DeclNode::Ptr decl) {
    repository->store(decl);
}

SourceFile::Ptr CompilerService::tryImport(const CompileContext::Ptr& context, const std::string &moduleName) {
    auto sourcefile = context->sourcefile;
    auto relativedFolder = sourcefile->getParentFolder();
    auto importfile = findSourceFile(moduleName, relativedFolder);
    if(importfile == nullptr) {
        Diagnostics::reportError("Error: Module cannot be found");
        return nullptr;
    }
    compile(importfile);
    
    CHECK_ERROR_RETURN_NULL
    return importfile;
}

void CompilerService::debugPrint(const Node::Ptr& node, const std::string &debugFilePath) {
    if(options->vmDebug) {
        NodeDebugPrinter syntaxDebugger(debugFilePath);
        syntaxDebugger.print(node);
        syntaxDebugger.close();
    }
}

void CompilerService::debugPrint(const std::string &debugFilePath) {
    if(options->vmDebug) {
        TypeTablePrinter typePrinter("debug.table.types.txt");
        typePrinter.print();
        typePrinter.close();
        
        FunctionTablePrinter funcPrinter("debug.table.functions.txt");
        funcPrinter.print();
        funcPrinter.close();
    }
}

void CompilerService::initializeGlobalSymbolTable() {
    globalSymbols = std::make_shared<SymbolTable>();
    auto symbolPrint = Symbol::make(SymbolFlag::funcSymbol, "&print(message:);");
    globalSymbols->insert(symbolPrint);
}
