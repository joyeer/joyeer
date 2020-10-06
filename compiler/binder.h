#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__


#include "node.h"
#include "context.h"

struct BinderDelegate {
    bool tryImport(const std::wstring module);
};

// Bind all types and symbols during analyzing AST tree
// Reduce the expression's depth level
class Binder: public NodeVisitor {
public:
    Binder(CompileContext::Ptr context);
    
    virtual Node::Ptr visit(SourceBlock::Ptr sourceBlock);
    
protected:
    // recursive bind down node
    virtual Node::Ptr visit(Node::Ptr node);
    virtual Node::Ptr visit(ClassDecl::Ptr classDecl);
    virtual Node::Ptr visit(LetDecl::Ptr decl);
    virtual Node::Ptr visit(VarDecl::Ptr varDecl);
    virtual Node::Ptr visit(ConstructorDecl::Ptr decl);
    virtual Node::Ptr visit(Type::Ptr decl);
    virtual Node::Ptr visit(FuncCallExpr::Ptr decl);
    virtual Node::Ptr visit(MemberFuncCallExpr::Ptr decl);
    virtual Node::Ptr visit(ArguCallExpr::Ptr decl);
    virtual Node::Ptr visit(LiteralExpr::Ptr decl);
    virtual Node::Ptr visit(PrefixExpr::Ptr decl);
    virtual Node::Ptr visit(IdentifierExpr::Ptr decl);
    virtual Node::Ptr visit(Expr::Ptr decl);
    virtual Node::Ptr visit(AssignmentExpr::Ptr decl);
    virtual Node::Ptr visit(BinaryExpr::Ptr decl);
    virtual Node::Ptr visit(OperatorExpr::Ptr decl);
    virtual Node::Ptr visit(ParenthesizedExpr::Ptr decl);
    virtual Node::Ptr visit(IfStatement::Ptr decl);
    virtual Node::Ptr visit(WhileStatement::Ptr decl);
    virtual Node::Ptr visit(CodeBlock::Ptr decl);
    virtual Node::Ptr visit(FuncDecl::Ptr decl);
    virtual Node::Ptr visit(ParameterClause::Ptr decl);
    virtual Node::Ptr visit(Pattern::Ptr decl);
    virtual Node::Ptr visit(ReturnStatement::Ptr decl);
    virtual Node::Ptr visit(SelfExpr::Ptr decl);
    virtual Node::Ptr visit(ArrayLiteralExpr::Ptr decl);
    virtual Node::Ptr visit(DictLiteralExpr::Ptr decl);
    virtual Node::Ptr visit(MemberAccessExpr::Ptr decl);
    virtual Node::Ptr visit(SubscriptExpr::Ptr decl);
    virtual Node::Ptr visit(ArrayType::Ptr decl);
    virtual Node::Ptr visit(FileImportDecl::Ptr decl);
private:
    CompileContext::Ptr context;
};

#endif
