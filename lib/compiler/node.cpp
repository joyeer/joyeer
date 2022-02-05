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

//--------------------------------------------------
// SelfExpr implementation
//--------------------------------------------------

SelfExpr::Ptr SelfExpr::create() {
    auto selfToken= std::make_shared<Token>(TokenKind::keyword, Keywords::SELF, -1, -1);
    auto selfIdentifier = std::make_shared<IdentifierExpr>(selfToken);
    return std::make_shared<SelfExpr>(selfIdentifier);
}

//--------------------------------------------------
// OptionalType implementation
//--------------------------------------------------

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

FuncDecl::FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, StmtsBlock::Ptr stmts) :
StmtsBlock(stmts->statements),
identifier(std::move(identifier)),
parameterClause(std::move(parameterClause)),
returnType(std::move(returnType)) {
    kind = SyntaxKind::funcDecl;
}


void FuncDecl::bindClass(const ClassDecl::Ptr &decl) {
    assert(symtable->find(Keywords::SELF) == nullptr);
    // self symbol

    assert(returnType == nullptr);

    auto clause = std::static_pointer_cast<ParameterClause>(this->parameterClause);

    auto selfIdentifier = std::make_shared<IdentifierExpr>(std::make_shared<Token>(TokenKind::keyword, Keywords::SELF, -1, -1));
    auto typeIdentifier = std::make_shared<IdentifierExpr>(std::make_shared<Token>(TokenKind::identifier, decl->getSimpleName(), -1, -1));
    auto pattern = std::make_shared<Pattern>(selfIdentifier, typeIdentifier);
    clause->parameters.push_back(pattern);
    
    // return type is ClassDecl
    auto token = std::make_shared<Token>(TokenKind::identifier, decl->getSimpleName(), -1, -1);
    returnType = std::make_shared<IdentifierExpr>(token);
}

FuncDecl::Ptr FuncDecl::createDefaultConstructor() {
    /**
     * default constructor looks like follow code:
     * init() {
     *   return self
     * }
     */
    auto parameterClause = std::make_shared<ParameterClause>( std::vector<Pattern::Ptr>());

    auto selfExpr = SelfExpr::create();
    auto returnStmt = std::make_shared<ReturnStmt>(selfExpr);
    auto stmts = std::make_shared<StmtsBlock>( std::vector<Node::Ptr> { returnStmt });

    return createConstructor(parameterClause, nullptr, stmts);
}

// create a Constructor FuncDecl
FuncDecl::Ptr FuncDecl::createConstructor( const Node::Ptr& parameterClause, const Node::Ptr& returnType, const StmtsBlock::Ptr& stmts) {
    auto token = std::make_shared<Token>(TokenKind::identifier, "init", -1, -1);
    auto nameIdentifier = std::make_shared<IdentifierExpr>(token);
    auto decl = std::make_shared<FuncDecl>(nameIdentifier, parameterClause, returnType, stmts);
    decl->isConstructor = true;
    return decl;
}

std::string FuncDecl::getSimpleName() {
    std::stringstream ss;

    // basis name
    if (identifier != nullptr) {
        ss << identifier->getSimpleName();
    }

    // parameters
    ss << DescriptorConstants::ParenthesisOpen;
    if (parameterClause) {
        for (const auto& p: std::static_pointer_cast<ParameterClause>(parameterClause)->parameters) {
            ss << p->getSimpleName() << DescriptorConstants::Colon;
        }
    }
    ss << DescriptorConstants::ParenthesisClose;

    return ss.str();
}

std::string FuncDecl::getSimpleConstructorName(const std::string& name) {
    std::stringstream ss;

    // basis name
    ss << name;

    // parameters
    ss << DescriptorConstants::ParenthesisOpen;
    if (parameterClause) {
        auto i = 0;
        for (const auto& p: std::static_pointer_cast<ParameterClause>(parameterClause)->parameters) {
            if(i == 0){
                continue;
            }
            ss << p->getSimpleName() << DescriptorConstants::Colon;
            i++;
        }
    }
    ss << DescriptorConstants::ParenthesisClose;

    return ss.str();
}