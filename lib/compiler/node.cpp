#include "joyeer/compiler/node.h"
#include "joyeer/runtime/buildin.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <cassert>


Node::Node(SyntaxKind k): kind(k) {
}

std::string Node::getName() {
    return "";
}

std::string Node::getTypeName() {
    return "";
}

OperatorExpr::OperatorExpr(Token::Ptr token):
Node(SyntaxKind::operatorExpr),
token(token) {

}

ParameterClause::ParameterClause(std::vector<std::shared_ptr<Pattern>> parameters):
Node(SyntaxKind::parameterClause),
parameters(parameters) {
}

Pattern::Pattern(IdentifierExpr::Ptr identifier, Node::Ptr type):
Node(SyntaxKind::pattern),
identifier(identifier),
type(type) {
}

const std::string& Pattern::getIdentifierName() {
    return identifier->token->rawValue;
}

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
    
    ss << parent->getTypeName() << "@" << member->getName() << "(";
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
    ss << parent->getTypeName() << "@" << member->getName();
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

std::string IdentifierExpr::getName() {
    return token->rawValue;
}

std::string IdentifierExpr::getTypeName() {
    if(symbol != nullptr && (symbol->flag == varSymbol || symbol->flag == fieldSymbol)) {
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

Type::Type(Node::Ptr identifier):
Node(SyntaxKind::type),
identifier(identifier) {
}

ArrayType::ArrayType(Node::Ptr type):
Node(SyntaxKind::arrayType),
type(type) {
}

std::string ArrayType::getTypeName() {
    return "Array@Array";
}

DictType::DictType(Node::Ptr keyType, Node::Ptr valueType):
Node(SyntaxKind::dictType),
keyType(keyType),
valueType(valueType) {
}

std::string DictType::getTypeName() {
    return "Map@Map";
}

FileImportStmt::FileImportStmt(Token::Ptr stringLiteral):
Node(SyntaxKind::fileimportStmt),
stringLiteral(stringLiteral) {
}

const std::string FileImportStmt::getImportedFilename() {
    return stringLiteral->rawValue;
}

ModuleMemberAccessExpr::ModuleMemberAccessExpr(JrModuleClass* moduleClass, Node::Ptr member):
Node(SyntaxKind::moduleMemberAccessExpr),
moduleClass(moduleClass),
member(member) {
}

ModuleFuncCallExpr::ModuleFuncCallExpr(JrModuleClass* moduleClass, Node::Ptr member):
Node(SyntaxKind::moduleFuncCallExpr),
moduleClass(moduleClass),
member(member) {
}

StmtsBlock::StmtsBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::stmtsBlock),
statements(statements) {
}

ForInStmt::ForInStmt(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock):
Node(SyntaxKind::forInStmt),
pattern(pattern),
inExpr(inExpr),
codeBlock(codeBlock) {
}

WhileStmt::WhileStmt(Node::Ptr expr, Node::Ptr codeBlock):
Node(SyntaxKind::whileStmt),
expr(expr),
codeBlock(codeBlock) {
}

IfStmt::IfStmt(Node::Ptr condition, Node::Ptr ifCodeBlock, std::shared_ptr<Node> elseCodeBlock):
Node(SyntaxKind::ifStmt),
condition(condition),
ifCodeBlock(ifCodeBlock),
elseCodeBlock(elseCodeBlock) {
}

ReturnStmt::ReturnStmt(Node::Ptr expr):
Node(SyntaxKind::returnStmt),
expr(expr) {
}
