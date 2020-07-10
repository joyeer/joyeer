#ifndef __joyeer_compiler_typecheck_h__
#define __joyeer_compiler_typecheck_h__

#include "node.h"
#include "context.h"
#include "runtime/runtime.h"

class TypeChecker {
public:
    TypeChecker(CompileContext::Pointer context);
    
    void verify(Node::Pointer node);
    
private:
    void verify(SourceBlock::Pointer node);
    void verify(FuncDecl::Pointer node);
    void verify(FuncCallExpr::Pointer node);
    void verify(VarDecl::Pointer node);
    void verify(ParameterClause::Pointer node);
    void verify(Pattern::Pointer node);
    void verify(IdentifierExpr::Pointer node);
    void verify(TypeDecl::Pointer node);
    void verify(CodeBlock::Pointer node);
    void verify(ReturnStatement::Pointer node);
    void verify(Expr::Pointer node);
    void verify(LetDecl::Pointer node);
    void verify(LiteralExpr::Pointer node);
    void verify(AssignmentExpr::Pointer node);
    void verify(ParenthesizedExpr::Pointer node);
    void verify(IfStatement::Pointer node);
    
    JrType::Pointer typeOf(Node::Pointer node);
    JrType::Pointer typeOf(IdentifierExpr::Pointer node);
    
    CompileContext::Pointer context;
};

#endif
