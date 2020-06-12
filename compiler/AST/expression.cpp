#include "ast.h"
#include "compiler/AST/ast.h"
#include "compiler/syntax/syntax.h"

Expr::Expr(SyntaxKind kind): Node(kind) {
    
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
