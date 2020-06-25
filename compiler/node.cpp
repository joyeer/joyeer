#include "node.h"

Node::Node(SyntaxKind k): kind(k) {

}

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

FuncCallExpr::FuncCallExpr(std::shared_ptr<Token> identifier, std::vector<std::shared_ptr<Node>> parameters):
Node(SyntaxKind::functionCallExpr),
identifier(identifier),
parameters(parameters) {
}

MemberExpr::MemberExpr(std::shared_ptr<Node> parent, std::shared_ptr<Node> member):
Node(SyntaxKind::memberExpr),
parent(parent),
member(member) {
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

SourceBlock::SourceBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::sourceBlock),
statements(statements) {

}

CodeBlock::CodeBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::codeBlock),
statements(statements) {
}

ForInStatement::ForInStatement(std::shared_ptr<Node> pattern, std::shared_ptr<Node> inExpr, std::shared_ptr<Node> codeBlock):
Node(SyntaxKind::forInStatement),
pattern(pattern),
inExpr(inExpr),
codeBlock(codeBlock) {
}

IfStatement::IfStatement(std::shared_ptr<Node> condition, std::shared_ptr<Node> ifCodeBlock, std::shared_ptr<Node> elseCodeBlock):
Node(SyntaxKind::ifStatement),
condition(condition),
ifCodeBlock(ifCodeBlock),
elseCodeBlock(elseCodeBlock) {

}
