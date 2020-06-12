#include "ast.h"
#include "compiler/AST/ast.h"
#include "compiler/syntax/syntax.h"

Expr::Expr(SyntaxKind kind): Node(kind) {
    
}

LiteralExpr::LiteralExpr(std::shared_ptr<Token> literal):
 Expr(SyntaxKind::literalExpr),
 literal(literal) {
    
}
