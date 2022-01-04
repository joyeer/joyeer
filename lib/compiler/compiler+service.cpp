#include "joyeer/compiler/compiler+service.h"

#include "joyeer/compiler/lexparser.h"
#include "joyeer/compiler/binder.h"
#include "joyeer/compiler/typecheck.h"
#include "joyeer/compiler/syntaxparser.h"
#include "joyeer/compiler/IRGen.h"
#include "debugprinter.h"
#include "joyeer/runtime/diagnostic.h"
#include "joyeer/runtime/types.h"

#include <utility>


#define CHECK_ERROR_RETURN_NULL \
    if(Diagnostics::errorLevel != none) { \
        return nullptr; \
    }

CompilerService::CompilerService(CommandLineArguments::Ptr opts):
        options(std::move(opts)) {
    globalSymbols = std::make_shared<SymbolTable>();
}

ModuleClass* CompilerService::run(const std::string& inputFile) {
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


ModuleClass* CompilerService::compile(const SourceFile::Ptr& sourcefile) {
    
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
    typedefPrinter.print(types->types);
    typedefPrinter.close();

    return sourcefile->moduleClass;
}

int CompilerService::declare(Type* type) {
    type->slot = static_cast<int32_t>(types->types.size());
    types->types.push_back(type);
    return type->slot;
}

Type* CompilerService::getType(int address) {
    assert(address >= 0 && address < types->types.size());
    return (*types)[address];
}

Type* CompilerService::getType(ValueType valueType) {
    return getType(static_cast<int>(valueType));
}

Type* CompilerService::getType(BuildIns buildIn) {
    return getType(static_cast<int>(buildIn));
}

SymbolTable::Ptr CompilerService::getExportingSymbolTable(int typeSlot) {
    assert(typeSlot != -1);
    return exportingSymbolTableOfClasses[typeSlot];
}

void CompilerService::debugPrint(const Node::Ptr& node, const std::string &debugFilePath) {
    if(options->vmDebug) {
        NodeDebugPrinter syntaxDebugger(debugFilePath);
        syntaxDebugger.print(node);
        syntaxDebugger.close();
    }
}

#define DECLARE_TYPE(type, TypeClass) \
    declare(new TypeClass()); \
    assert((type) == types->types.back()->kind);  \
    assert((size_t)(type) == (types->types.size() - 1));

#define DECLARE_FUNC(type, param, typeSlot) \
    { \
        auto func = new Function(param); \
        func->funcKind = FuncTypeKind::C_Func;         \
        func->returnTypeSlot = (int)typeSlot; \
        declare(func); \
    }

#define BEGIN_DECLARE_CLASS(type, TypeClass) \
    { \
        auto symtable = std::make_shared<SymbolTable>(); \
        auto typeClass = new TypeClass(); \
        declare(typeClass); \
        globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::klass, typeClass->name, typeClass->slot)); \
        assert((size_t)(type) == typeClass->slot); \
        exportingSymbolTableOfClasses.insert({ typeClass->slot, symtable });

#define DECLARE_CLASS_FUNC(name, typeSlot) \
    { \
        auto func = new Function(name); \
        func->funcKind = FuncTypeKind::C_Func;        \
        func->returnTypeSlot = (int)(typeSlot);  \
        auto funcAddress = declare(func); \
        symtable->insert(std::make_shared<Symbol>(SymbolFlag::func, name, funcAddress)); \
    }

#define END_DECLARE_CLASS(type) \
    }

void CompilerService::bootstrap() {

    DECLARE_TYPE(ValueType::Void, VoidType)
    DECLARE_TYPE(ValueType::Int, IntType)
    DECLARE_TYPE(ValueType::Bool, BoolType)
    DECLARE_TYPE(ValueType::Nil, NilType)
    DECLARE_TYPE(ValueType::Unspecified, UnspecifiedType)
    DECLARE_TYPE(ValueType::Any, AnyType)

    DECLARE_FUNC(BuildIns::Func_Print, "print(message:)", ValueType::Void)

    // Declare build-in classes and its members
    BEGIN_DECLARE_CLASS(BuildIns::Object_Array, ArrayClass)
        DECLARE_CLASS_FUNC("append(content:)", ValueType::Void)
        DECLARE_CLASS_FUNC("size()", ValueType::Int)
        DECLARE_CLASS_FUNC("get(index:)", ValueType::Any)
        DECLARE_CLASS_FUNC("set(index:value:)", ValueType::Void)
    END_DECLARE_CLASS("Array")

    auto print = getType(BuildIns::Func_Print);
    globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::func, print->name, print->slot));
    globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::klass, "Int", (int)ValueType::Int));
    globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::klass, "Void", (int)ValueType::Void));
    globalSymbols->insert(std::make_shared<Symbol>(SymbolFlag::klass, "Bool", (int)ValueType::Bool));

}
