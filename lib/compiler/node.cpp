#include "joyeer/compiler/node.h"

#include <utility>

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
token(std::move(token)) {

}

ParameterClause::ParameterClause(std::vector<Pattern::Ptr> parameters):
Node(SyntaxKind::parameterClause),
parameters(std::move(parameters)) {
}

Pattern::Pattern(IdentifierExpr::Ptr identifier, Node::Ptr type):
        Node(SyntaxKind::pattern),
        identifier(std::move(identifier)),
        typeExpr(std::move(type)) {
}

const std::string& Pattern::getIdentifierName() const {
    return identifier->token->rawValue;
}

TypeIdentifier::TypeIdentifier(Node::Ptr identifier):
Node(SyntaxKind::type),
identifier(std::move(identifier)) {
}

ArrayType::ArrayType(Node::Ptr type):
Node(SyntaxKind::arrayType),
valueType(std::move(type)) {
}


DictType::DictType(Node::Ptr keyType, Node::Ptr valueType):
Node(SyntaxKind::dictType),
keyType(std::move(keyType)),
valueType(std::move(valueType)) {
}

/**
 * OptionalType implementation
 */

OptionalType::OptionalType(Node::Ptr type, bool required):
Node(SyntaxKind::optionalType),
type(std::move(type)),
required(required) {
}

std::string OptionalType::getSimpleName() {
    return "Optional<" + type->getSimpleName() + ">";
}
