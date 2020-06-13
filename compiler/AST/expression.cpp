#include "ast.h"
#include "compiler/AST/ast.h"
#include "compiler/syntax/syntax.h"

Expr::Expr(SyntaxKind kind): Node(kind) {
    
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

FunctionCallExpr::FunctionCallExpr(std::vector<std::shared_ptr<Node>> parameters, std::shared_ptr<Token> postfixOperator):
    Node(SyntaxKind::functionCallExpr),
    parameters(parameters),
    postfixOperator(postfixOperator) {
}

LiteralExpr::LiteralExpr(std::shared_ptr<Token> literal):
    Expr(SyntaxKind::literalExpr),
    literal(literal) {
    
}

IdentifierExpr::IdentifierExpr(std::shared_ptr<Token> identifier):
  Expr(SyntaxKind::identifierExpr),
  identifier(identifier) {
}

ParenthesizedExpr::ParenthesizedExpr(std::shared_ptr<Expr> expr):
  Expr(SyntaxKind::parenthesizedExpr),
  expr(expr) {

}
