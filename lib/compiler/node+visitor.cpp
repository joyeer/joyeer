#include "joyeer/compiler/node+visitor.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Node::Ptr NodeVisitor::visit(Node::Ptr node) {
    switch (node->kind) {
        case SyntaxKind::filemodule:
            return visit(std::static_pointer_cast<FileModuleDecl>(node));
        case SyntaxKind::type:
            return visit(std::static_pointer_cast<Type>(node));
        case SyntaxKind::pattern:
            return visit(std::static_pointer_cast<Pattern>(node));
        case SyntaxKind::varDecl:
            return visit(std::static_pointer_cast<VarDecl>(node));
        case SyntaxKind::funcDecl:
            return visit(std::static_pointer_cast<FuncDecl>(node));
        case SyntaxKind::constructorDecl:
            return visit(std::static_pointer_cast<ConstructorDecl>(node));
        case SyntaxKind::classDecl:
            return visit(std::static_pointer_cast<ClassDecl>(node));
        case SyntaxKind::parameterClause:
            return visit(std::static_pointer_cast<ParameterClause>(node));
        case SyntaxKind::stmtsBlock:
            return visit(std::static_pointer_cast<StmtsBlock>(node));
        case SyntaxKind::ifStmt:
            return visit(std::static_pointer_cast<IfStmt>(node));
        case SyntaxKind::whileStmt:
            return visit(std::static_pointer_cast<WhileStmt>(node));
        case SyntaxKind::expr:
            return visit(std::static_pointer_cast<Expr>(node));
        case SyntaxKind::selfExpr:
            return visit(std::static_pointer_cast<SelfExpr>(node));
        case SyntaxKind::prefixExpr:
            return visit(std::static_pointer_cast<PrefixExpr>(node));
        case SyntaxKind::identifierExpr:
            return visit(std::static_pointer_cast<IdentifierExpr>(node));
        case SyntaxKind::parenthesizedExpr:
            return visit(std::static_pointer_cast<ParenthesizedExpr>(node));
        case SyntaxKind::arguCallExpr:
            return visit(std::static_pointer_cast<ArguCallExpr>(node));
        case SyntaxKind::funcCallExpr:
            return visit(std::static_pointer_cast<FuncCallExpr>(node));
        case SyntaxKind::memberAccessExpr:
            return visit(std::static_pointer_cast<MemberAccessExpr>(node));
        case SyntaxKind::memberFuncCallExpr:
            return visit(std::static_pointer_cast<MemberFuncCallExpr>(node));
        case SyntaxKind::literalExpr:
            return visit(std::static_pointer_cast<LiteralExpr>(node));
        case SyntaxKind::arrayLiteralExpr:
            return visit(std::static_pointer_cast<ArrayLiteralExpr>(node));
        case SyntaxKind::dictLiteralExpr:
            return visit(std::static_pointer_cast<DictLiteralExpr>(node));
        case SyntaxKind::assignmentExpr:
            return visit(std::static_pointer_cast<AssignmentExpr>(node));
        case SyntaxKind::binaryExpr:
            return visit(std::static_pointer_cast<BinaryExpr>(node));
        case SyntaxKind::operatorExpr:
            return visit(std::static_pointer_cast<OperatorExpr>(node));
        case SyntaxKind::returnStmt:
            return visit(std::static_pointer_cast<ReturnStmt>(node));
        case SyntaxKind::subscriptExpr:
            return visit(std::static_pointer_cast<SubscriptExpr>(node));
        case SyntaxKind::arrayType:
            return visit(std::static_pointer_cast<ArrayType>(node));
        case SyntaxKind::fileimportStmt:
            return visit(std::static_pointer_cast<FileImportStmt>(node));
        case SyntaxKind::moduleMemberAccessExpr:
            return visit(std::static_pointer_cast<ModuleMemberAccessExpr>(node));
        case SyntaxKind::moduleFuncCallExpr:
            return visit(std::static_pointer_cast<ModuleFuncCallExpr>(node));
        default:
            assert(false);
    }
}
