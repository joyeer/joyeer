#ifndef __joyeer_compiler_context_h__
#define __joyeer_compiler_context_h__

#include "compiler/compileopts.h"
#include "symtable.h"
#include "runtime/function.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

enum CompileStage {
    visitSourceBlock,
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
    
    CompileContext(CompileOpts::Ptr options);
    
    SymbolTable::Ptr initializeSymTable();
    void finalizeSymTable();
    
    void visit(CompileStage stage, std::function<void(void)> visit);
    CompileStage curStage() const;
    
    // look up name's mapped symbol
    Symbol::Ptr lookup(const std::wstring& name);
    
    SymbolTable::Ptr curSymTable() ;
    void entry(SymbolTable::Ptr table);
    void leave(SymbolTable::Ptr table);
    
    JrFunction* curFunction();
    void entry(JrFunction* function);
    void leave(JrFunction* function);
    
    JrType* curType();
    void entry(JrType* type);
    void leave(JrType* type);
    
    // Associate Type with symbol table
    void associate(JrType*, SymbolTable::Ptr table);
    // return assoicated symbol table with type
    SymbolTable::Ptr symtableOfType(JrType*);
    
protected:
    // initializ global scope, e.g. buildin functions/object etc
    void initializeGlobalScope();
    
    // entry into an stage of compile pipeline
    void entry(CompileStage stage);
    
    // leave from an compile pipeline stage
    void leave(CompileStage stage);
    
protected:
    std::vector<SymbolTable::Ptr> symbols;
    
    // Copmpiling stage
    std::vector<CompileStage> stages;
    
    // parsing function stacks
    std::vector<JrFunction*> functions;
    
    // parsing class/struct/enum stacks
    std::vector<JrType*> types;
    
    std::unordered_map<JrInt, SymbolTable::Ptr> mapOfTypeAndSymbolTable;
    
    CompileOpts::Ptr options;
};

#endif

