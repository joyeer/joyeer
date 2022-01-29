#include "joyeer/compiler/node.h"

#include <utility>

Node::Node(SyntaxKind k): kind(k) {
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

//-------------------------------------
//FuncDecl implementation
//-------------------------------------

FuncDecl::Ptr FuncDecl::makeDefaultConstructor(const ClassDecl::Ptr& decl) {
    /**
     * default constructor looks like follow code:
     * init() {
     *   return self
     * }
     */
    auto parameterClause = std::make_shared<ParameterClause>( std::vector<Pattern::Ptr>());
    auto selfToken= std::make_shared<Token>(TokenKind::keyword, Keywords::SELF, -1, -1);
    auto selfIdentifier = std::make_shared<IdentifierExpr>(selfToken);
    auto selfExpr = std::make_shared<SelfExpr>(selfIdentifier);
    auto returnStmt = std::make_shared<ReturnStmt>(selfExpr);
    auto stmts = std::make_shared<StmtsBlock>( std::vector<Node::Ptr> { returnStmt });

    auto className = decl->getSimpleName();
    auto returnTypeIdentifier = std::make_shared<IdentifierExpr>(std::make_shared<Token>(TokenKind::identifier, className, -1, -1));

    return makeConstructor(parameterClause, returnTypeIdentifier, stmts);
}

// create a Constructor FuncDecl
FuncDecl::Ptr FuncDecl::makeConstructor(const Node::Ptr& parameterClause, const Node::Ptr& returnType, const StmtsBlock::Ptr& stmts) {
    auto token = std::make_shared<Token>(TokenKind::identifier, "init", -1, -1);
    auto nameIdentifier = std::make_shared<IdentifierExpr>(token);
    auto decl = std::make_shared<FuncDecl>(nameIdentifier, parameterClause, returnType, stmts);
    decl->isConstructor = true;
    return decl;
}


