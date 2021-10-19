#ifndef __joyeer_compiler_irgen_h__
#define __joyeer_compiler_irgen_h__

#include "joyeer/compiler/context.h"
#include "joyeer/compiler/typedef.h"
#include "joyeer/compiler/opcode.h"

class IRGen {
public:
    explicit IRGen(CompileContext::Ptr context);

    JrModuleTypeDef::Ptr emit(const FileModuleDecl::Ptr& block);
    
protected:
    void emit(const Node::Ptr& node);
    void emit(const FuncCallExpr::Ptr& node);
    void emit(const MemberFuncCallExpr::Ptr& node);
    void emit(const ArguCallExpr::Ptr& node);
    void emit(const LiteralExpr::Ptr& node);
    void emit(const VarDecl::Ptr& node);
    void emit(const PrefixExpr::Ptr& node);
    void emit(const IdentifierExpr::Ptr& node);
    void emit(const AssignmentExpr::Ptr& node);
    void emit(const Expr::Ptr& node);
    void emit(const OperatorExpr::Ptr& node);
    void emit(const ParenthesizedExpr::Ptr& node);
    void emit(const IfStmt::Ptr& node);
    void emit(const WhileStmt::Ptr& node);
    void emit(const StmtsBlock::Ptr& node);
    void emit(const FuncDecl::Ptr& node);
    void emit(const ReturnStmt::Ptr& node);
    void emit(const MemberAccessExpr::Ptr& node);
    void emit(const MemberAssignExpr::Ptr& node);
    void emit(const ClassDecl::Ptr& node);
    void emit(const ArrayLiteralExpr::Ptr& node);
    void emit(const DictLiteralExpr::Ptr& node);
    void emit(const SubscriptExpr::Ptr& node);
    void emit(const FileImportStmt::Ptr& node);
    
private:
    JrCodeWriter writer;
    CompileContext::Ptr context;
};

#endif
