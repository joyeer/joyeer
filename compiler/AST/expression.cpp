#include "ast.h"
#include "compiler/AST/ast.h"
#include "compiler/syntax/syntax.h"

Expr::Expr(std::shared_ptr<Node> prefix, std::shared_ptr<Node> binary):
Node(SyntaxKind::expr),
prefix(prefix),
binary(binary) {
}

PostfixExpr::PostfixExpr(std::shared_ptr<Node> expr, std::shared_ptr<Token> postfixOperator):
    Node(SyntaxKind::postfixExpr),
    expr(expr),
    postfixOperator(postfixOperator) {
}

PrefixExpr::PrefixExpr(std::shared_ptr<Token> prefixOperator, std::shared_ptr<Node> expr):
    Node(SyntaxKind::prefixExpr),
    prefixOperator(prefixOperator),
    expr(expr) {
}

BinaryExpr::BinaryExpr(std::shared_ptr<Token> binaryOperator, std::shared_ptr<Node> expr):
    Node(SyntaxKind::binaryOperator),
    binaryOperator(binaryOperator),
    expr(expr) {
}

AssignmentExpr::AssignmentExpr(std::shared_ptr<Node> expr):
    Node(SyntaxKind::assignmentOperator),
    expr(expr) {
}

ArguCallExpr::ArguCallExpr(std::shared_ptr<Token> identifier, std::shared_ptr<Node> expr):
Node(SyntaxKind::arguCallExpr),
identifier(identifier),
expr(expr) {
}

FunctionCallExpr::FunctionCallExpr(std::shared_ptr<Token> identifier, std::vector<std::shared_ptr<Node>> parameters):
Node(SyntaxKind::functionCallExpr),
identifier(identifier),
parameters(parameters) {
}

LiteralExpr::LiteralExpr(std::shared_ptr<Token> literal):
Node(SyntaxKind::literalExpr),
literal(literal) {
    
}

IdentifierExpr::IdentifierExpr(std::shared_ptr<Token> identifier):
Node(SyntaxKind::identifierExpr),
identifier(identifier) {
}

ParenthesizedExpr::ParenthesizedExpr(std::shared_ptr<Node> expr):
Node(SyntaxKind::parenthesizedExpr),
expr(expr) {
}

SelfExpr::SelfExpr(std::shared_ptr<Token> identifier):
Node(SyntaxKind::selfExpr),
identifier(identifier) {
}

Type::Type(std::shared_ptr<Token> identifier, bool isOptional):
Node(SyntaxKind::type),
identifier(identifier),
isOptional(isOptional) {
}
