#include "decl.h"



ConstDecl::ConstDecl(std::shared_ptr<Node> pattern, std::shared_ptr<Node> initializer):
Node(SyntaxKind::constantDecl),
pattern(pattern),
initializer(initializer) {
}

VarDecl::VarDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer):
Node(SyntaxKind::varDecl),
pattern(pattern),
initializer(initializer) {
    
}

ClassDecl::ClassDecl(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Node>> members):
Node(SyntaxKind::classDecl),
name(name),
members(members) {
}

ParameterClause::ParameterClause(std::vector<std::shared_ptr<Node>> parameters):
Node(SyntaxKind::parameterClause),
parameters(parameters) {
}

FuncDecl::FuncDecl(std::shared_ptr<Token> name, std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock):
Node(SyntaxKind::funcDecl),
name(name),
parameterClause(parameterClause),
codeBlock(codeBlock) {
}

Pattern::Pattern(std::shared_ptr<Token> identifier, Type::Pointer type):
Node(SyntaxKind::pattern),
identifier(identifier),
type(type) {
}

ConstructorDecl::ConstructorDecl(std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock):
Node(SyntaxKind::constructorDecl),
parameterClause(parameterClause),
codeBlock(codeBlock) {
}
