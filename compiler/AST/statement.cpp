#include "ast.h"

ConstantDecl::ConstantDecl(std::shared_ptr<Token> identifier, std::shared_ptr<Node> initializer): 
Node(SyntaxKind::consantDecl),
identifier(identifier),
initializer(initializer) {

}