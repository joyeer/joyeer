#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__


#include "node.h"
#include "context.h"


// Bind all types and symbols during analyzing AST tree
// Reduce the expression's depth level
class Binder {
public:
    Binder(CompileContext::Pointer context);
    
    SourceBlock::Pointer bind(SourceBlock::Pointer sourceBlock);
    
protected:
    // recursive bind down node
    Node::Pointer bind(Node::Pointer node);
    
    Node::Pointer bind(ClassDecl::Pointer classDecl);
    
    Node::Pointer bind(LetDecl::Pointer decl);
    
    Node::Pointer bind(VarDecl::Pointer varDecl);
    
    Node::Pointer bind(ConstructorDecl::Pointer decl);
    
    Node::Pointer bind(TypeDecl::Pointer decl);
    
    Node::Pointer bind(FuncCallExpr::Pointer decl);
    
    Node::Pointer bind(ArguCallExpr::Pointer decl);
    
    Node::Pointer bind(LiteralExpr::Pointer decl);
    
    Node::Pointer bind(PrefixExpr::Pointer decl);
    
    Node::Pointer bind(IdentifierExpr::Pointer decl);
    
    Node::Pointer bind(Expr::Pointer decl);
    
    Node::Pointer bind(AssignmentExpr::Pointer decl);
    
    Node::Pointer bind(BinaryExpr::Pointer decl);
    
    Node::Pointer bind(OperatorExpr::Pointer decl);
    
    Node::Pointer bind(ParenthesizedExpr::Pointer decl);
    
    Node::Pointer bind(IfStatement::Pointer decl);
    
    Node::Pointer bind(CodeBlock::Pointer decl);
    
    Node::Pointer bind(FuncDecl::Pointer decl);
    
    Node::Pointer bind(ParameterClause::Pointer decl);
    
    Node::Pointer bind(Pattern::Pointer decl);
    
    Node::Pointer bind(ReturnStatement::Pointer decl);
    
private:
    CompileContext::Pointer context;
};

#endif
