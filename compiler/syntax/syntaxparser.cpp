#include "syntaxparser.h"
#include "compiler/AST/ast.h"

SyntaxParser::SyntaxParser(const std::vector<std::shared_ptr<Token>> &tokens) : tokens(tokens)
{
    iterator = tokens.begin();
    endIterator = tokens.end();
}

std::shared_ptr<SourceFile> SyntaxParser::parse()
{
    return nullptr;
}

void SyntaxParser::parseDecl()
{
}

void SyntaxParser::tryParseConstDecl()
{
    if (tryEat(TokenKind::keyword, Keywords::LET) == nullptr)
    {
        return;
    }

    std::shared_ptr<Token> identifier = tryParseIdentifier();
    if (identifier == nullptr)
    {
        return; //Error
    }

    if (tryEat(TokenKind::operators, Operators::EQULAS) != nullptr)
    {
    }
}

void SyntaxParser::tryParseType()
{
}

void SyntaxParser::tryParseClassDecl()
{
    if (tryEat(TokenKind::keyword, Keywords::CLASS) == nullptr)
    {
        return;
    }

    std::shared_ptr<Token> className = tryEat(TokenKind::identifier);
    if (className == nullptr)
    {
        return; // TODO: Error
    }

    if (tryEat(TokenKind::punctuation, Punctuations::OPEN_CURLY_BRACKET) == nullptr)
    {
        return; // TODO: Error
    }

    if (tryEat(TokenKind::punctuation, Punctuations::CLOSE_CURLY_BRACKET) == nullptr)
    {
        return; // TODO: Error
    }
}

// expression -> prefix-expression /opt/ binary-expressions /opt/
void SyntaxParser::tryParseExpr()
{
}

void SyntaxParser::tryParsePrefixExpr()
{
}

void SyntaxParser::tryParsePostfixExpr()
{
}

void SyntaxParser::tryParseBinaryExpr()
{
}

void SyntaxParser::tryParseBinaryOperator()
{
}

std::shared_ptr<Node> SyntaxParser::tryParsePrimaryExpr()
{
    std::shared_ptr<Token> identifier = tryParseIdentifier();
    if (identifier != nullptr)
    {
        return std::shared_ptr<Node>(new IdentifierExpr(identifier));
    }

    std::shared_ptr<Node> literalExpr = tryParseLiteralExpr();
    if (literalExpr != nullptr)
    {
        return literalExpr;
    }

    return nullptr;
}

std::shared_ptr<LiteralExpr> SyntaxParser::tryParseLiteralExpr()
{
    std::shared_ptr<Token> literal = tryParseLiteral();
    if (literal != nullptr)
    {
        return std::shared_ptr<LiteralExpr>(new LiteralExpr(literal));
    }
    return nullptr;
}

void SyntaxParser::tryParseConditionalOperator()
{
}

std::shared_ptr<Node> SyntaxParser::tryParseAssignmentOperator()
{
    if (tryEat(TokenKind::operators, Operators::EQULAS) == nullptr)
    {
        return nullptr;
    }
    return std::shared_ptr<Node>(new AssignmentOperator());
}

std::shared_ptr<Token> SyntaxParser::tryParseOperator()
{
    return tryEat(TokenKind::operators);
}

std::shared_ptr<Token> SyntaxParser::tryParseIdentifier()
{
    return tryEat(TokenKind::identifier);
}

std::shared_ptr<Token> SyntaxParser::tryParseLiteral()
{
    std::shared_ptr<Token> literal = tryEat(TokenKind::stringLiteral);
    if (literal != nullptr)
    {
        return literal;
    }

    literal = tryEat(TokenKind::decimalLiteral);
    if (literal != nullptr)
    {
        return literal;
    }

    literal = tryEat(TokenKind::floatLiteral);
    if (literal != nullptr)
    {
        return nullptr;
    }

    literal = tryEat(TokenKind::nilLiteral);
    if (literal != nullptr)
    {
        return nullptr;
    }

    literal = tryEat(TokenKind::booleanLiteral);
    if (literal != nullptr)
    {
        return nullptr;
    }

    return nullptr;
}

std::shared_ptr<Token> SyntaxParser::tryEat(TokenKind kind)
{

    if (iterator != endIterator)
    {
        std::shared_ptr<Token> pToken = curToken();
        if (pToken->kind == kind)
        {
            iterator++;
            return pToken;
        }
    }

    return nullptr;
}

std::shared_ptr<Token> SyntaxParser::tryEat(TokenKind kind, const std::wstring &value)
{

    std::shared_ptr<Token> pToken = tryEat(kind);
    if (pToken != nullptr && pToken->value == value)
    {
        return pToken;
    }

    return nullptr;
}

std::shared_ptr<Token> SyntaxParser::curToken() const
{
    return *iterator;
}
