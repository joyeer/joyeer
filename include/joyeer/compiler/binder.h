#ifndef __joyeer_compiler_syntax_binder_h__
#define __joyeer_compiler_syntax_binder_h__


#include "joyeer/compiler/node+visitor.h"
#include "joyeer/compiler/context.h"


struct SourceFile;

// Bind all types and symbols during analyzing AST tree
// Reduce the expression's depth level
class Binder: public NodeVisitor {
public:
    std::function<SourceFile::Ptr(CompileContext::Ptr, const std::string&)> importDelegate;
    
    explicit Binder(CompileContext::Ptr context);
    
    Node::Ptr visit(FileModuleDecl::Ptr decl) override;
protected:
    // recursive bind down node
    Node::Ptr visit(const Node::Ptr& node) override;
    Node::Ptr visit(ClassDecl::Ptr classDecl) override;
    Node::Ptr visit(VarDecl::Ptr varDecl) override;
    Node::Ptr visit(Type::Ptr decl) override;
    Node::Ptr visit(FuncCallExpr::Ptr decl) override;
    Node::Ptr visit(MemberFuncCallExpr::Ptr decl) override;
    Node::Ptr visit(ArguCallExpr::Ptr decl) override;
    Node::Ptr visit(LiteralExpr::Ptr decl) override;
    Node::Ptr visit(PrefixExpr::Ptr decl) override;
    Node::Ptr visit(IdentifierExpr::Ptr decl) override;
    Node::Ptr visit(Expr::Ptr decl) override;
    Node::Ptr visit(AssignExpr::Ptr decl) override;
    Node::Ptr visit(BinaryExpr::Ptr decl) override;
    Node::Ptr visit(OperatorExpr::Ptr decl) override;
    Node::Ptr visit(ParenthesizedExpr::Ptr decl) override;
    Node::Ptr visit(IfStmt::Ptr decl) override;
    Node::Ptr visit(WhileStmt::Ptr decl) override;
    Node::Ptr visit(StmtsBlock::Ptr decl) override;
    Node::Ptr visit(FuncDecl::Ptr decl) override;
    Node::Ptr visit(ParameterClause::Ptr decl) override;
    Node::Ptr visit(Pattern::Ptr decl) override;
    Node::Ptr visit(ReturnStmt::Ptr decl) override;
    Node::Ptr visit(SelfExpr::Ptr decl) override;
    Node::Ptr visit(ArrayLiteralExpr::Ptr decl) override;
    Node::Ptr visit(DictLiteralExpr::Ptr decl) override;
    Node::Ptr visit(MemberAccessExpr::Ptr decl) override;
    Node::Ptr visit(MemberAssignExpr::Ptr decl) override;
    Node::Ptr visit(SubscriptExpr::Ptr decl) override;
    Node::Ptr visit(ArrayType::Ptr decl) override;
    Node::Ptr visit(FileImportStmt::Ptr decl) override;
    
private:
    /**
     Normalize the FileModule's statements & declarations, group into an ConstructorDecl
     FileModuleNode
        + FileImportStatement
        + statement1
        + statement2
        + declaration1
        + declaration2
        + statement3
    after normalized --->
     FileModuleNode
        + FileImportStatement
        + ConstructorDecl
            + statement1
            + statement2
            + statement3
        + declaration1
        + declaration2
    */
    FileModuleDecl::Ptr normalizeAndPrepareDefaultStaticConstructorForFileModule(FileModuleDecl::Ptr filemodule);
    
    // process the ClassDecl & FileModuleNode
    void processClassDecl(ClassDecl::Ptr decl);
    
private:
    CompileContext::Ptr context;
};

#endif
