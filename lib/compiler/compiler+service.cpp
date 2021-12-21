#include "joyeer/compiler/compiler+service.h"
#include "joyeer/compiler/lexparser.h"
#include "joyeer/common/diagnostic.h"
#include "joyeer/compiler/binder.h"
#include "joyeer/compiler/typecheck.h"
#include "joyeer/compiler/syntaxparser.h"
#include "joyeer/compiler/IRGen.h"
#include "debugprinter.h"
#include <utility>


#define CHECK_ERROR_RETURN_NULL \
    if(Diagnostics::errorLevel != none) { \
        return nullptr; \
    }

CompilerService::CompilerService(CommandLineArguments::Ptr opts):
        options(std::move(opts)) {
    initializeTypes();
    initializeGlobalSymbolTable();
}

ModuleType::Ptr CompilerService::run(const std::string& inputFile) {
    auto sourcefile = findSourceFile(inputFile);
    return compile(sourcefile);
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


ModuleType::Ptr CompilerService::compile(const SourceFile::Ptr& sourcefile) {
    
    auto context= std::make_shared<CompileContext>(options, globalSymbols);
    context->sourcefile = sourcefile;
    context->compiler = this;
    
    // lex structure analyze
    LexParser lexParser;
    lexParser.parse(sourcefile);
    CHECK_ERROR_RETURN_NULL
    
    // syntax analyze
    SyntaxParser syntaxParser(sourcefile);
    auto block = syntaxParser.parse();
    block->filename = sourcefile->getAbstractLocation();
    debugPrint(block, block->filename + ".parser.debug.yml");
    CHECK_ERROR_RETURN_NULL
    
    // Detect for kind creating
    Binder binder(context);
    binder.visit(block);
    debugPrint(block, block->filename + ".binder.debug.yml");
    CHECK_ERROR_RETURN_NULL
    
    // verify the types
    TypeChecker typeChecker(context);
    typeChecker.visit(std::static_pointer_cast<Node>(block));
    debugPrint(block, sourcefile->getAbstractLocation() + ".typechecker.debug.yml");
    CHECK_ERROR_RETURN_NULL

    // generate IR code
    IRGen irGen(context);
    sourcefile->moduleClass = irGen.emit(block);
    CHECK_ERROR_RETURN_NULL

    // debug print the typedefs
    TypeDefDebugPrinter typedefPrinter(sourcefile->getAbstractLocation() + ".typedef.debug.yml");
    typedefPrinter.print(types);
    typedefPrinter.close();

    return sourcefile->moduleClass;
}

void CompilerService::declare(const Type::Ptr& type) {
    type.get()->address = static_cast<int32_t>(types.size());
    types.push_back(type);
}

Type::Ptr CompilerService::getType(int address) {
    return types[address];
}

Type::Ptr CompilerService::getType(ValueType valueType) {
    return types[static_cast<int>(valueType)];
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
    auto print = getBuildInsType(BuildIns::Func_Print);
    globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::func, print->name, print->address));
    globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::klass, "Int", (int)ValueType::Int));
    globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::klass, "Void", (int)ValueType::Void));
    globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::klass, "Bool", (int)ValueType::Bool));
}

void CompilerService::initializeTypes() {

#define DECLARE_TYPE(type, TypeClass) \
    declare(std::make_shared<TypeClass>()); \
    assert((type) == types.back()->kind);  \
    assert((size_t)(type) == (types.size() - 1));

#define DECLARE_TYPE_2(type, TypeClass, param) \
    declare(std::make_shared<TypeClass>(param));

    DECLARE_TYPE(ValueType::Void, VoidType)
    DECLARE_TYPE(ValueType::Int, IntType)
    DECLARE_TYPE(ValueType::Bool, BoolType)
    DECLARE_TYPE(ValueType::Nil, NilType)
    DECLARE_TYPE(ValueType::Unspecified, UnspecifiedType)
    DECLARE_TYPE(ValueType::Any, AnyType)

    DECLARE_TYPE_2(BuildIns::Func_Print, FuncType, "print(message:)")
}
