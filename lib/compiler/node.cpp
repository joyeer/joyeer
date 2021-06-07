#include "joyeer/compiler/node.h"
#include "joyeer/runtime/buildin.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <cassert>


static int generateVID() {
    static int vid = 1;
    return vid ++;
}

Node::Node(SyntaxKind k): kind(k) {
    vid = generateVID();
}

std::string Node::getSimpleName() {
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


DictType::DictType(Node::Ptr keyType, Node::Ptr valueType):
Node(SyntaxKind::dictType),
keyType(keyType),
valueType(valueType) {
}
