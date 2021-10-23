#ifndef __joyeer_compiler_context_h__
#define __joyeer_compiler_context_h__

#include "joyeer/driver/arguments.h"
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
    
    explicit CompileContext(CommandLineArguments::Ptr options);
    
    void visit(CompileStage stage, const std::function<void(void)>& visit);
    void visit(CompileStage stage, const Node::Ptr& node, const std::function<void(void)>& visit);

    [[nodiscard]] CompileStage curStage() const;
    
    // look up name's mapped symbol
    Symbol::Ptr lookup(const std::string& name);
    
    SymbolTable::Ptr curSymTable() ;
    void entry(const SymbolTable::Ptr& table);
    void leave(const SymbolTable::Ptr& table);

    // return the current FuncDef in the stack
    [[nodiscard]] JrFuncTypeDef::Ptr curFuncDef() const;

    // return the current JrModuleTypeDef in the stack
    [[nodiscard]] JrFileModuleTypeDef::Ptr curModuleDef() const;

    // the current parsing source file
    SourceFile::Ptr sourcefile = nullptr;
    
    // The global compiler service
    CompilerService* compiler{};
    
protected:

    // the parsing stack
    std::vector<Node::Ptr> parsingStack;

    std::vector<SymbolTable::Ptr> symbols;
    
    // stage of compiling status, the last element of stages present the current stage
    std::vector<CompileStage> stages;

    // parsing scope's descriptor stacks
    std::stack<Descriptor::Ptr> descriptors;
    
    CommandLineArguments::Ptr options;
    
};

#endif

