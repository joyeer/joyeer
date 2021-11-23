#ifndef __joyeer_compiler_context_h__
#define __joyeer_compiler_context_h__

#include "joyeer/common/arguments.h"
#include "joyeer/compiler/sourcefile.h"

class CompilerService;

enum class CompileStage : uint64_t {
    visitFileModule  = 1,
    visitCodeBlock,
    visitClassDecl,
    visitVarDecl,
    visitLetDecl,
    visitFuncDecl,          // process func declaration
    visitFuncNameDecl,      // process func name
    visitFuncParamDecl,
    visitFuncCall,
    visitMemberFuncCall,
    visitMemberAccess,
    visitAssignExpr,
    visitExpr,
};

class CompileContext {
public:
    typedef std::shared_ptr<CompileContext> Ptr;
    
    CompileContext(CommandLineArguments::Ptr options, const SymbolTable::Ptr& globalSymTable);

    void visit(CompileStage stage, const Node::Ptr& node, const std::function<void(void)>& visit);

    [[nodiscard]] CompileStage curStage() const;
    
    // look up name's mapped symbol
    Symbol::Ptr lookup(const std::string& name);
    
    SymbolTable::Ptr curSymTable() ;

    [[nodiscard]] Type::Ptr curTypeDef() const;

    // Return the Top declarations Types - FileModuleDef/FuncDef/ClassDef
    [[nodiscard]] Type::Ptr  curDeclTypeDef() const ;

    // return the current FuncDef in the stack
    [[nodiscard]] FuncType::Ptr curFuncDef() const;

    // return the current JrModuleTypeDef in the stack
    [[nodiscard]] ModuleType::Ptr curModuleDef() const;

    // return the current BlockDef in stack
    [[nodiscard]] BlockType::Ptr curBlockDef() const;

    // the current parsing source file
    SourceFile::Ptr sourcefile = nullptr;
    
    // The global compiler service
    CompilerService* compiler;
    
protected:
    // Symbol Tables in parsing stack, basics, once dive into next AST, will push symbol table into stack
    std::vector<SymbolTable::Ptr> symbols;

    // the parsing stack
    std::vector<Type::Ptr> types;
    
    // stage of compiling status, the last element of stages present the current stage
    std::vector<CompileStage> stages;

    CommandLineArguments::Ptr options;
    
};

#endif

