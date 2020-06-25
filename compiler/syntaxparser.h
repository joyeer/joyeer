#ifndef __joyeer_syntax_syntaxparser_h__
#define __joyeer_syntax_syntaxparser_h__

#include "node.h"

class SyntaxParser
{
public:
    SyntaxParser(const std::vector<std::shared_ptr<Token>> &tokens);
    SourceBlock::Pointer parse();

private:

    ////////////////////////////////////////////////////////////////////////////
    // Declarations
    ////////////////////////////////////////////////////////////////////////////
    Node::Pointer tryParseDecl();

    Node::Pointer tryParseClassDecl();

    Node::Pointer tryParseFunctionDecl();
    
    Node::Pointer tryParseConstructorDecl();

    // Parse the constant declaration, e.g. let constValue = "10"
    Node::Pointer tryParseConstDecl();

    // Parse the variable declaration, e.g. var varValue = "10"
    std::shared_ptr<Node> tryParseVarDecl();
    
    // Parse the parameter clause
    std::shared_ptr<Node> tryParseParameterClause();

    ////////////////////////////////////////////////////////////////////////////
    // Statements
    ////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Node> tryParseCodeBlock();

    std::shared_ptr<Node> tryParseStatement();

    std::shared_ptr<Node> tryParseLoopStatement();

    std::shared_ptr<Node> tryParseBranchStatement();

    std::shared_ptr<Node> tryParseIfStatement();

    ////////////////////////////////////////////////////////////////////////////
    // Type
    ////////////////////////////////////////////////////////////////////////////
    Pattern::Pointer tryParsePattern();
    
    Type::Pointer tryParseTypeAnnotation();
    
    Type::Pointer tryParseType();

    ////////////////////////////////////////////////////////////////////////////
    // Expressions
    ////////////////////////////////////////////////////////////////////////////

    Node::Pointer tryParseExpr();

    // prefix-expression -> prefix-operator /opt/ postfix-expression
    std::shared_ptr<Node> tryParsePrefixExpr();

    // postfix-expression -> primary-expression
    // postfix-expression -> postfix-expression postfix-operator
    // postfix-expression -> function-call-expression
    std::shared_ptr<Node> tryParsePostfixExpr();
    
    ArguCallExpr::Pointer tryParseArguCallExpr();
    
    std::shared_ptr<Node> tryParseFuncCallExpr();

    std::shared_ptr<Node> tryParseBinaryExpr();

    // primary-expression -> identifier
    // primary-expression -> literal-expression
    // primary-expression -> parenthesized-expression
    std::shared_ptr<Node> tryParsePrimaryExpr();

    std::shared_ptr<Node> tryParseSelfExpr();
    
    // literal-expression -> literal
    // literal-expression -> array-literal | dictionary-literal
    // array-literal -> [ array-literal-items /opts/ ]
    // array-literal-items -> array-literal-item , /opt/ |  array-literal-item, array-literal-items
    // dictionary-literal -> [ dictionary-literal-items ]  | [ : ]
    // dictionary-literal-items -> dictionary-literal-item, /opt/ | dictionary-literal-item, dictionary-literal-items
    // dictionary-literal-item -> expression: expression
    std::shared_ptr<LiteralExpr> tryParseLiteralExpr();

    // parenthesized-expression -> `(` expression `)`
    std::shared_ptr<Node> tryParseParenthesizedExpr();

    void tryParseConditionalOperator();
    
    Token::Pointer tryParseOperator();
    
    Token::Pointer tryParsePostfixOperator();

    // literal -> numeric-literal | string-literal | boolean-literal | nil-literal
    std::shared_ptr<Token> tryParseLiteral();
    //
    std::shared_ptr<Token> tryParseIdentifier();


    ////////////////////////////////////////////////////////////////////////////
    // Helper functions
    ////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<Token> tryEat(TokenKind kind, const std::wstring &value);

    std::shared_ptr<Token> tryEat(TokenKind kind);

    void previous();

    std::shared_ptr<Token> curToken() const;

private:
    const std::vector<std::shared_ptr<Token>> &tokens;
    std::vector<std::shared_ptr<Token>>::const_iterator iterator;
    std::vector<std::shared_ptr<Token>>::const_iterator endIterator;
};

#endif
