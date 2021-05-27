#include "joyeer/compiler/node.h"
#include "joyeer/runtime/buildin.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <cassert>


Node::Node(SyntaxKind k): kind(k) {
}

std::string Node::queryName() {
    return "";
}

std::string Node::getTypeName() {
    return "";
}

OperatorExpr::OperatorExpr(Token::Ptr token):
Node(SyntaxKind::operatorExpr),
token(token) {

}

ParameterClause::ParameterClause(std::vector<Pattern::Ptr> parameters):
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
