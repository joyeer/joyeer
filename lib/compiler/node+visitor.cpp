#include "joyeer/compiler/node+visitor.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Node::Ptr NodeVisitor::visit(Node::Ptr node) {
    switch (node->kind) {
        case sourceBlock:
            return visit(std::static_pointer_cast<FileModuleNode>(node));
        case type:
            return visit(std::static_pointer_cast<Type>(node));
        case pattern:
            return visit(std::static_pointer_cast<Pattern>(node));
        case letDecl:
            return visit(std::static_pointer_cast<LetDecl>(node));
        case varDecl:
            return visit(std::static_pointer_cast<VarDecl>(node));
        case funcDecl:
            return visit(std::static_pointer_cast<FuncDecl>(node));
        case constructorDecl:
            return visit(std::static_pointer_cast<ConstructorDecl>(node));
        case classDecl:
            return visit(std::static_pointer_cast<ClassDecl>(node));
        case parameterClause:
            return visit(std::static_pointer_cast<ParameterClause>(node));
        case codeBlock:
            return visit(std::static_pointer_cast<CodeBlock>(node));
        case ifStatement:
            return visit(std::static_pointer_cast<IfStatement>(node));
        case whileStatement:
            return visit(std::static_pointer_cast<WhileStatement>(node));
        case expr:
            return visit(std::static_pointer_cast<Expr>(node));
        case selfExpr:
            return visit(std::static_pointer_cast<SelfExpr>(node));
        case prefixExpr:
            return visit(std::static_pointer_cast<PrefixExpr>(node));
        case identifierExpr:
            return visit(std::static_pointer_cast<IdentifierExpr>(node));
        case parenthesizedExpr:
            return visit(std::static_pointer_cast<ParenthesizedExpr>(node));
        case arguCallExpr:
            return visit(std::static_pointer_cast<ArguCallExpr>(node));
        case funcCallExpr:
            return visit(std::static_pointer_cast<FuncCallExpr>(node));
        case memberAccessExpr:
            return visit(std::static_pointer_cast<MemberAccessExpr>(node));
        case memberFuncCallExpr:
            return visit(std::static_pointer_cast<MemberFuncCallExpr>(node));
        case literalExpr:
            return visit(std::static_pointer_cast<LiteralExpr>(node));
        case arrayLiteralExpr:
            return visit(std::static_pointer_cast<ArrayLiteralExpr>(node));
        case dictLiteralExpr:
            return visit(std::static_pointer_cast<DictLiteralExpr>(node));
        case assignmentExpr:
            return visit(std::static_pointer_cast<AssignmentExpr>(node));
        case binaryExpr:
            return visit(std::static_pointer_cast<BinaryExpr>(node));
        case operatorExpr:
            return visit(std::static_pointer_cast<OperatorExpr>(node));
        case returnStatement:
            return visit(std::static_pointer_cast<ReturnStatement>(node));
        case subscriptExpr:
            return visit(std::static_pointer_cast<SubscriptExpr>(node));
        case arrayType:
            return visit(std::static_pointer_cast<ArrayType>(node));
        case fileimportDecl:
            return visit(std::static_pointer_cast<FileImportDecl>(node));
        case moduleMemberAccessExpr:
            return visit(std::static_pointer_cast<ModuleMemberAccessExpr>(node));
        case moduleFuncCallExpr:
            return visit(std::static_pointer_cast<ModuleFuncCallExpr>(node));
        default:
            assert(false);
    }
}