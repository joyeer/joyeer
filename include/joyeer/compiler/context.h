#ifndef __joyeer_compiler_context_h__
#define __joyeer_compiler_context_h__

#include "joyeer/runtime/arguments.h"
#include "joyeer/compiler/sourcefile.h"

class CompilerService;

enum class CompileStage : uint64_t {
    visitModule  = 1,
    visitCodeBlock,
    visitClassDecl,
    visitVarDecl,
    visitFuncDecl,          // process func declaration
    visitFuncNameDecl,      // process func name
    visitFuncParamDecl,
    visitFuncCall,
    visitMemberAccess,
    visitMemberFuncCall,
    visitAssignExpr,
    visitExpr,
};

class CompileContext {
public:
    typedef std::shared_ptr<CompileContext> Ptr;
    
    CompileContext(CommandLineArguments::Ptr options, const SymbolTable::Ptr& globalSymTable);

    void visit(CompileStage stage, const Node::Ptr& nodde, const std::function<void(void)>& visit);

    [[nodiscard]] CompileStage curStage() const;
    
    // look up name's mapped symbol
    Symbol::Ptr lookup(const std::string& name);
    
    SymbolTable::Ptr curSymTable() ;

    // Return the Top declarations Types - ModuleType/FuncType/ClassType
    [[nodiscard]] Type*  curDeclType() const ;

    // return the current FuncType in the stack
    [[nodiscard]] FuncType* curFuncType() const;

    // return the current BlockType in the stack
    [[nodiscard]] BlockType* curBlockType() const;

    // return the current ModuleType in the stack
    [[nodiscard]] ModuleType* curModuleType() const;

    // the current parsing source file
    SourceFile::Ptr sourcefile = nullptr;
    
    // The global compiler service
    CompilerService* compiler;
    
protected:
    // Symbol Tables in parsing stack, basics, once dive into next AST, will push symbol table into stack
    std::vector<SymbolTable::Ptr> symbols;

    // the parsing stack
    std::vector<int> types;
    
    // stage of compiling status, the last element of stages present the current stage
    std::vector<CompileStage> stages;

    CommandLineArguments::Ptr options;
    
};

#endif

