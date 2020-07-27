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
    Node::Pointer tryParseVarDecl();
    
    // Parse the parameter clause
    Node::Pointer tryParseParameterClause();

    ////////////////////////////////////////////////////////////////////////////
    // Statements
    ////////////////////////////////////////////////////////////////////////////

    Node::Pointer tryParseCodeBlock();

    Node::Pointer tryParseStatement();

    Node::Pointer tryParseLoopStatement();

    Node::Pointer tryParseBranchStatement();

    Node::Pointer tryParseIfStatement();
    
    Node::Pointer tryParseReturnStatement();

    ////////////////////////////////////////////////////////////////////////////
    // Type
    ////////////////////////////////////////////////////////////////////////////
    Pattern::Pointer tryParsePattern();
    
    TypeDecl::Pointer tryParseTypeAnnotation();
    
    TypeDecl::Pointer tryParseType();

    ////////////////////////////////////////////////////////////////////////////
    // Expressions
    ////////////////////////////////////////////////////////////////////////////

    Node::Pointer tryParseExpr();

    // prefix-expression -> prefix-operator /opt/ postfix-expression
    Node::Pointer tryParsePrefixExpr();

    // postfix-expression -> primary-expression
    // postfix-expression -> postfix-expression postfix-operator
    // postfix-expression -> function-call-expression
    // postfix-expression -> subscript-expression
    Node::Pointer tryParsePostfixExpr();
    
    // try parse 
    Node::Pointer tryParseSubscriptExpr();
    
    ArguCallExpr::Pointer tryParseArguCallExpr();
    
    Node::Pointer tryParseFuncCallExpr();

    Node::Pointer tryParseBinaryExpr();

    // primary-expression -> identifier
    // primary-expression -> literal-expression
    // primary-expression -> parenthesized-expression
    Node::Pointer tryParsePrimaryExpr();

    Node::Pointer tryParseSelfExpr();
    
    // literal-expression -> literal
    // literal-expression -> array-literal | dictionary-literal
    // array-literal -> [ array-literal-items /opts/ ]
    // array-literal-items -> array-literal-item , /opt/ |  array-literal-item, array-literal-items
    // dictionary-literal -> [ dictionary-literal-items ]  | [ : ]
    // dictionary-literal-items -> dictionary-literal-item, /opt/ | dictionary-literal-item, dictionary-literal-items
    // dictionary-literal-item -> expression: expression
    Node::Pointer tryParseLiteralExpr();
    Node::Pointer tryParseArrayLiteralExpr();

    // parenthesized-expression -> `(` expression `)`
    Node::Pointer tryParseParenthesizedExpr();

    void tryParseConditionalOperator();
    
    OperatorExpr::Pointer tryParseOperatorExpr();
    
    OperatorExpr::Pointer tryParsePostfixOperatorExpr();
    
    // literal -> numeric-literal | string-literal | boolean-literal | nil-literal
    Token::Pointer tryParseLiteral();
    //
    IdentifierExpr::Pointer tryParseIdentifierExpr();


    ////////////////////////////////////////////////////////////////////////////
    // Helper functions
    ////////////////////////////////////////////////////////////////////////////
    Token::Pointer tryEat(TokenKind kind, const std::wstring &value);

    Token::Pointer tryEat(TokenKind kind);

    void previous();

    Token::Pointer curToken() const;

private:
    const std::vector<Token::Pointer> &tokens;
    std::vector<Token::Pointer>::const_iterator iterator;
    std::vector<Token::Pointer>::const_iterator endIterator;
};

#endif
