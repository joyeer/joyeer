#include "joyeer/compiler/node+expr.h"
#include "joyeer/runtime/buildin.h"
#include <sstream>

Expr::Expr(Node::Ptr prefix, std::vector<Node::Ptr> binaries):
Node(SyntaxKind::expr),
prefix(prefix),
binaries(binaries) {
}

Expr::Expr(std::vector<Node::Ptr> nodes):
Node(SyntaxKind::expr),
prefix(nullptr),
binaries(),
nodes(nodes){
    
}

PostfixExpr::PostfixExpr(Node::Ptr expr, OperatorExpr::Ptr op):
Node(SyntaxKind::postfixExpr),
expr(expr),
op(op) {
}

PrefixExpr::PrefixExpr(OperatorExpr::Ptr op, Node::Ptr expr):
Node(SyntaxKind::prefixExpr),
op(op),
expr(expr) {
    
}

BinaryExpr::BinaryExpr(OperatorExpr::Ptr op, Node::Ptr expr):
Node(SyntaxKind::binaryExpr),
op(op),
expr(expr) {
}

AssignmentExpr::AssignmentExpr(Node::Ptr expr):
Node(SyntaxKind::assignmentExpr),
expr(expr) {
}

ArguCallExpr::ArguCallExpr(IdentifierExpr::Ptr identifier, Node::Ptr expr):
Node(SyntaxKind::arguCallExpr),
label(identifier),
expr(expr) {
}

FuncCallExpr::FuncCallExpr(Node::Ptr expr, std::vector<ArguCallExpr::Ptr> arguments):
Node(SyntaxKind::funcCallExpr),
identifier(expr),
arguments(arguments) {
}

std::string FuncCallExpr::getTypeName() {
    std::stringstream ss;

    ss << identifier->getTypeName() << "(";
    for(auto& argument: arguments) {
        ss << argument->label->token->rawValue;
        ss << ":";
    }
    ss << ")";
    return ss.str();
}

MemberFuncCallExpr::MemberFuncCallExpr(Node::Ptr parent, Node::Ptr member, std::vector<ArguCallExpr::Ptr> arguments):
Node(SyntaxKind::memberFuncCallExpr),
parent(parent),
member(member),
arguments(arguments) {
}

std::string MemberFuncCallExpr::getTypeName() {
    std::stringstream ss;
    
    ss << parent->getTypeName() << "@" << member->queryName() << "(";
    for(auto& argument: arguments) {
        ss << argument->label->token->rawValue << ":";
    }
    ss << ")";
    return ss.str();
}

MemberAccessExpr::MemberAccessExpr(std::shared_ptr<Node> parent, std::shared_ptr<Node> member):
Node(SyntaxKind::memberAccessExpr),
parent(parent),
member(member) {
}

std::string MemberAccessExpr::getTypeName() {
    std::stringstream ss;
    ss << parent->getTypeName() << "@" << member->queryName();
    return ss.str();
}

LiteralExpr::LiteralExpr(std::shared_ptr<Token> literal):
Node(SyntaxKind::literalExpr),
literal(literal) {
    
}

ArrayLiteralExpr::ArrayLiteralExpr(std::vector<Node::Ptr> items):
Node(SyntaxKind::arrayLiteralExpr),
items(items) {
}

DictLiteralExpr::DictLiteralExpr(std::vector<std::tuple<Node::Ptr, Node::Ptr>> items):
Node(SyntaxKind::dictLiteralExpr),
items(items) {
}

IdentifierExpr::IdentifierExpr(std::shared_ptr<Token> token):
Node(SyntaxKind::identifierExpr),
token(token) {
}

std::string IdentifierExpr::queryName() {
    return token->rawValue;
}

std::string IdentifierExpr::getTypeName() {
    if(symbol != nullptr && (symbol->flag == SymbolFlag::varSymbol || symbol->flag == SymbolFlag::fieldSymbol)) {
        auto type = Global::types[symbol->addressOfType];
        assert(type != nullptr);
        return type->name;
    } else {
        return token->rawValue;
    }
    
    assert(false);
}

ParenthesizedExpr::ParenthesizedExpr(std::shared_ptr<Node> expr):
Node(SyntaxKind::parenthesizedExpr),
expr(expr) {
}

SelfExpr::SelfExpr(IdentifierExpr::Ptr identifier):
Node(SyntaxKind::selfExpr),
identifier(identifier) {
}

SubscriptExpr::SubscriptExpr(Node::Ptr identifier, Node::Ptr indexExpr):
Node(SyntaxKind::subscriptExpr),
identifier(identifier),
indexExpr(indexExpr) {
}
