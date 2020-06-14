#include "decl.h"



ConstantDecl::ConstantDecl(std::shared_ptr<Token> identifier, std::shared_ptr<Node> initializer): 
Node(SyntaxKind::constantDecl),
identifier(identifier),
initializer(initializer) {

}

VarDecl::VarDecl(std::shared_ptr<Token> identifier, std::shared_ptr<Node> initializer):
Node(SyntaxKind::varDecl),
identifier(identifier),
initializer(initializer) {
    
}
