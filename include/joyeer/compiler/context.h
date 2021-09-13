#ifndef __joyeer_compiler_context_h__
#define __joyeer_compiler_context_h__

#include "joyeer/driver/arguments.h"
#include "joyeer/compiler/symtable.h"
#include "joyeer/compiler/sourcefile.h"
#include "joyeer/runtime/function.h"

class CompilerService;

enum class CompileStage {
    visitFileModule,
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
    
    CompileContext(CommandLineArguments::Ptr options);
    
    void visit(CompileStage stage, std::function<void(void)> visit);
    void visit(CompileStage stage, Node::Ptr node, std::function<void(void)> visit);
    
    Descriptor::Ptr parentDescriptor();
    
    CompileStage curStage() const;
    
    // look up name's mapped symbol
    Symbol::Ptr lookup(const std::string& name);
    
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
    void associate(JrType* type, SymbolTable::Ptr table);
    // return assoicated symbol table with type
    SymbolTable::Ptr symtableOfType(JrType*);
   
    // the current parsing source file
    SourceFile::Ptr sourcefile = nullptr;
    
    // The global compiler service
    CompilerService* compiler;
    
protected:
    std::vector<SymbolTable::Ptr> symbols;
    
    // stage of compiling status, the last element of stages present the current stage
    std::vector<CompileStage> stages;
    
    // parsing function stacks
    std::vector<JrFunction*> functions;
    
    // parsing class/struct/enum stacks
    std::vector<JrType*> types;
    
    // parsing scope's descriptor stacks
    std::stack<Descriptor::Ptr> descriptors;
    
    std::unordered_map<JrInt, SymbolTable::Ptr> mapOfTypeAndSymbolTable;
    
    CommandLineArguments::Ptr options;
    
};

#endif

