#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__


#include "node.h"
#include "context.h"


// Bind all types and symbols during analyzing AST tree
// Reduce the expression's depth level
class Binder {
public:
    Binder(CompileContext::Ptr context);
    
    SourceBlock::Ptr bind(SourceBlock::Ptr sourceBlock);
    
protected:
    // recursive bind down node
    Node::Ptr bind(Node::Ptr node);
    Node::Ptr bind(ClassDecl::Ptr classDecl);
    Node::Ptr bind(LetDecl::Ptr decl);
    Node::Ptr bind(VarDecl::Ptr varDecl);
    Node::Ptr bind(ConstructorDecl::Ptr decl);
    Node::Ptr bind(Type::Ptr decl);
    Node::Ptr bind(FuncCallExpr::Ptr decl);
    Node::Ptr bind(MemberFuncCallExpr::Ptr decl);
    Node::Ptr bind(ArguCallExpr::Ptr decl);
    Node::Ptr bind(LiteralExpr::Ptr decl);
    Node::Ptr bind(PrefixExpr::Ptr decl);
    Node::Ptr bind(IdentifierExpr::Ptr decl);
    Node::Ptr bind(Expr::Ptr decl);
    Node::Ptr bind(AssignmentExpr::Ptr decl);
    Node::Ptr bind(BinaryExpr::Ptr decl);
    Node::Ptr bind(OperatorExpr::Ptr decl);
    Node::Ptr bind(ParenthesizedExpr::Ptr decl);
    Node::Ptr bind(IfStatement::Ptr decl);
    Node::Ptr bind(WhileStatement::Ptr decl);
    Node::Ptr bind(CodeBlock::Ptr decl);
    Node::Ptr bind(FuncDecl::Ptr decl);
    Node::Ptr bind(ParameterClause::Ptr decl);
    Node::Ptr bind(Pattern::Ptr decl);
    Node::Ptr bind(ReturnStatement::Ptr decl);
    Node::Ptr bind(SelfExpr::Ptr decl);
    Node::Ptr bind(ArrayLiteralExpr::Ptr decl);
    Node::Ptr bind(DictLiteralExpr::Ptr decl);
    Node::Ptr bind(MemberAccessExpr::Ptr decl);
    Node::Ptr bind(SubscriptExpr::Ptr decl);
    Node::Ptr bind(ArrayType::Ptr decl);
    Node::Ptr bind(FileImportDecl::Ptr decl);
private:
    CompileContext::Ptr context;
};

#endif
