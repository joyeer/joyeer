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

ClassDecl::ClassDecl(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Node>> members):
Node(SyntaxKind::classDecl),
name(name),
members(members) {
}

FuncDecl::FuncDecl(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Token>> parameters, std::shared_ptr<Node> codeBlock):
Node(SyntaxKind::funcDecl),
name(name),
parameters(parameters),
codeBlock(codeBlock) {
}
