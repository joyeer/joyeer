#ifndef __joyeer_compiler_context_h__
#define __joyeer_compiler_context_h__

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
    visitAssignExpr,
    visitExpr,
};

class CompileContext {
public:
    typedef std::shared_ptr<CompileContext> Pointer;
    
    CompileContext();
    
    SymbolTable::Pointer initializeSymTable();
    void finalizeSymTable();
    
    void visit(CompileStage stage, std::function<void(void)> visit);
    CompileStage curStage() const;
    
    // look up name's mapped symbol
    Symbol::Pointer lookup(const std::wstring& name);
    
    SymbolTable::Pointer curSymTable() ;
    void entry(SymbolTable::Pointer table);
    void leave(SymbolTable::Pointer table);
    
    JrFunction::Pointer curFunction();
    void entry(JrFunction::Pointer function);
    void leave(JrFunction::Pointer function);
    
    JrType::Pointer curType();
    void entry(JrType::Pointer type);
    void leave(JrType::Pointer type);
    
    // Associate Type with symbol table
    void associate(JrType::Pointer, SymbolTable::Pointer table);
    // return assoicated symbol table with type
    SymbolTable::Pointer symtableOfType(JrType::Pointer);
    
protected:
    // initializ global scope, e.g. buildin functions/object etc
    void initializeGlobalScope();
    
    // entry into an stage of compile pipeline
    void entry(CompileStage stage);
    
    // leave from an compile pipeline stage
    void leave(CompileStage stage);
    
protected:
    std::vector<SymbolTable::Pointer> symbols;
    
    // Copmpiling stage
    std::vector<CompileStage> stages;
    
    // parsing function stacks
    std::vector<JrFunction::Pointer> functions;
    
    // parsing class/struct/enum stacks
    std::vector<JrType::Pointer> types;
    
    std::unordered_map<JrInt, SymbolTable::Pointer> mapOfTypeAndSymbolTable;
};

#endif

