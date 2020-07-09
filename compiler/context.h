#ifndef __joyeer_compiler_context_h__
#define __joyeer_compiler_context_h__

#include "symtable.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

enum ScopeFlag {
    globalScope,
    sourceScope,
    classScope,
    funcScope,
    codeBlockScope,
};

enum CompileStage {
    visitSourceBlock,
    visitCodeBlock,
    visitClassDecl,
    visitFuncDecl,          // process func declaration
    visitFuncNameDecl,      // process func name
    visitFuncParamDecl,
    
    visitFuncCall,
    visitFuncParamsCall,
    visitAssignExpr,
    visitExpr,
};

class CompileContext {
public:
    typedef std::shared_ptr<CompileContext> Pointer;
    
    CompileContext();
    
    // initialize an new scope, e.g. when parsing an function/class, we will initialize an scope,
    // basically, an scope is a Symtable
    void initializeScope(ScopeFlag flag);
    
    // complete an scope
    void finalizeScope(ScopeFlag flag);
    
    void visit(CompileStage stage, std::function<void(void)> visit);
    
    CompileStage curStage() const;
    
    // look up name's mapped symbol
    Symbol::Pointer lookup(const std::wstring& name);
    
    // insert an new Symbol in top symbol table
    bool insert(Symbol::Pointer symbol);
    
    SymbolTable::Pointer curSymTable() ;
    
    void entry(SymbolTable::Pointer table);
    
    void leave(SymbolTable::Pointer table);
    
protected:
    // initializ global scope, e.g. buildin functions/object etc
    void initializeGlobalScope();
    
    // entry into an stage of compile pipeline
    void entry(CompileStage stage);
    
    // leave from an compile pipeline stage
    void leave(CompileStage stage);
    
protected:
    std::vector<SymbolTable::Pointer> symbols;
    std::vector<ScopeFlag> scopes;
    std::vector<CompileStage> stages;
};

#endif

