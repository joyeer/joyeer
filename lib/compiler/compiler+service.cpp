#include "joyeer/compiler/compiler+service.h"
#include "joyeer/compiler/lexparser.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/compiler/binder.h"
#include "joyeer/compiler/typecheck.h"
#include "joyeer/compiler/syntaxparser.h"
#include "joyeer/compiler/IRGen.h"
#include "debugprinter.h"
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
    initializeTypeDefs();
    initializeGlobalSymbolTable();
}

void CompilerService::run(const std::string& inputFile) {
    auto sourcefile = findSourceFile(inputFile);
    compile(sourcefile);
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
    
    auto context= std::make_shared<CompileContext>(options, globalSymbols);
    context->sourcefile = sourcefile;
    context->compiler = this;
    
    // lex structure analyze
    LexParser lexParser;
    lexParser.parse(sourcefile);
    CHECK_ERROR_CONTINUE
    
    // syntax analyze
    SyntaxParser syntaxParser(sourcefile);
    auto block = syntaxParser.parse();
    block->filename = sourcefile->getAbstractLocation();
    debugPrint(block, block->filename + ".parser.debug.yml");
    CHECK_ERROR_CONTINUE
    
    // Detect for kind creating
    Binder binder(context);
    binder.importDelegate = std::bind(&CompilerService::tryImport, this, std::placeholders::_1, std::placeholders::_2);
    
    binder.visit(block);
    debugPrint(block, block->filename + ".binder.debug.yml");
    CHECK_ERROR_CONTINUE
    
    // verify the types
    TypeChecker typeChecker(context);
    typeChecker.visit(std::static_pointer_cast<Node>(block));
    debugPrint(block, sourcefile->getAbstractLocation() + ".typechecker.debug.yml");
    CHECK_ERROR_CONTINUE

    // generate IR code
    IRGen irGen(context);
    sourcefile->moduleClass = irGen.emit(block);
    CHECK_ERROR_CONTINUE

}

void CompilerService::declare(const JrTypeDef::Ptr& type) {
    type->address = static_cast<int32_t>(types.size());
    types.push_back(type);
}

JrTypeDef::Ptr CompilerService::getTypeDefBy(int address) {
    return types[address];
}

SourceFile::Ptr CompilerService::tryImport(const CompileContext::Ptr& context, const std::string &moduleName) {
    auto sourcefile = context->sourcefile;
    auto relativedFolder = sourcefile->getParentFolder();
    auto importfile = findSourceFile(moduleName, relativedFolder);
    if(importfile == nullptr) {
        Diagnostics::reportError("Error: FileModule cannot be found");
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

void CompilerService::initializeGlobalSymbolTable() {
    globalSymbols = std::make_shared<SymbolTable>();
    globalSymbols->insert(Symbol::make(SymbolFlag::func, BuildIn::TypeDef::print->name, BuildIn::TypeDef::print->address));
}

void CompilerService::initializeTypeDefs() {
    declare(BuildIn::TypeDef::Void);
    declare(BuildIn::TypeDef::Any);
    declare(BuildIn::TypeDef::Nil);
    declare(BuildIn::TypeDef::Int);
    declare(BuildIn::TypeDef::Bool);

    declare(BuildIn::TypeDef::print);
}
