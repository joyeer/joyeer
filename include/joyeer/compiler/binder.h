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
    
    Binder(CompileContext::Ptr context);
    
    virtual Node::Ptr visit(FileModuleDecl::Ptr sourceBlock);
protected:
    // recursive bind down node
    virtual Node::Ptr visit(Node::Ptr node);
    virtual Node::Ptr visit(ClassDecl::Ptr classDecl);
    virtual Node::Ptr visit(VarDecl::Ptr varDecl);
    virtual Node::Ptr visit(Type::Ptr decl);
    virtual Node::Ptr visit(FuncCallExpr::Ptr decl);
    virtual Node::Ptr visit(MemberFuncCallExpr::Ptr decl);
    virtual Node::Ptr visit(ModuleMemberAccessExpr::Ptr decl);
    virtual Node::Ptr visit(ModuleFuncCallExpr::Ptr decl);
    virtual Node::Ptr visit(ArguCallExpr::Ptr decl);
    virtual Node::Ptr visit(LiteralExpr::Ptr decl);
    virtual Node::Ptr visit(PrefixExpr::Ptr decl);
    virtual Node::Ptr visit(IdentifierExpr::Ptr decl);
    virtual Node::Ptr visit(Expr::Ptr decl);
    virtual Node::Ptr visit(AssignmentExpr::Ptr decl);
    virtual Node::Ptr visit(BinaryExpr::Ptr decl);
    virtual Node::Ptr visit(OperatorExpr::Ptr decl);
    virtual Node::Ptr visit(ParenthesizedExpr::Ptr decl);
    virtual Node::Ptr visit(IfStmt::Ptr decl);
    virtual Node::Ptr visit(WhileStmt::Ptr decl);
    virtual Node::Ptr visit(StmtsBlock::Ptr decl);
    virtual Node::Ptr visit(FuncDecl::Ptr decl);
    virtual Node::Ptr visit(ParameterClause::Ptr decl);
    virtual Node::Ptr visit(Pattern::Ptr decl);
    virtual Node::Ptr visit(ReturnStmt::Ptr decl);
    virtual Node::Ptr visit(SelfExpr::Ptr decl);
    virtual Node::Ptr visit(ArrayLiteralExpr::Ptr decl);
    virtual Node::Ptr visit(DictLiteralExpr::Ptr decl);
    virtual Node::Ptr visit(MemberAccessExpr::Ptr decl);
    virtual Node::Ptr visit(SubscriptExpr::Ptr decl);
    virtual Node::Ptr visit(ArrayType::Ptr decl);
    virtual Node::Ptr visit(FileImportStmt::Ptr decl);
    
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
    
    // prepare default constructor for class decl
    FuncDecl::Ptr prepareDefaultConstructorForClass(ClassDecl::Ptr decl);
    
private:
    CompileContext::Ptr context;
};

#endif
