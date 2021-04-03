#ifndef __joyeer_syntax_syntaxparser_h__
#define __joyeer_syntax_syntaxparser_h__

#include "node.h"

class SyntaxParser
{
public:
    SyntaxParser(const std::vector<std::shared_ptr<Token>> &tokens);
    SourceBlock::Ptr parse();

private:

    ////////////////////////////////////////////////////////////////////////////
    // Declarations
    ////////////////////////////////////////////////////////////////////////////
    Node::Ptr tryParseDecl();

    Node::Ptr tryParseClassDecl();

    Node::Ptr tryParseFunctionDecl();
    
    Node::Ptr tryParseConstructorDecl();

    // Parse the constant declaration, e.g. let constValue = "10"
    Node::Ptr tryParseConstDecl();

    // Parse the variable declaration, e.g. var varValue = "10"
    Node::Ptr tryParseVarDecl();
    
    // Parse the parameter clause
    Node::Ptr tryParseParameterClause();
    
    // Parse the fileimport delcaration;
    Node::Ptr tryParseFileImportDecl();

    ////////////////////////////////////////////////////////////////////////////
    // Statements
    ////////////////////////////////////////////////////////////////////////////

    Node::Ptr tryParseCodeBlock();
    Node::Ptr tryParseStatement();
    Node::Ptr tryParseLoopStatement();
    Node::Ptr tryparseWhileStatement();
    Node::Ptr tryParseBranchStatement();
    Node::Ptr tryParseIfStatement();
    Node::Ptr tryParseReturnStatement();

    ////////////////////////////////////////////////////////////////////////////
    // Type
    ////////////////////////////////////////////////////////////////////////////
    Pattern::Ptr tryParsePattern();
    
    Node::Ptr tryParseTypeAnnotation();
    Node::Ptr tryParseTypeIdentifier();
    Node::Ptr tryParseTypeArray();
    
    Node::Ptr tryParseType();

    ////////////////////////////////////////////////////////////////////////////
    // Expressions02079
    ////////////////////////////////////////////////////////////////////////////

    Node::Ptr tryParseExpr();

    // prefix-expression -> prefix-operator /opt/ postfix-expression
    Node::Ptr tryParsePrefixExpr();

    // postfix-expression -> primary-expression
    // postfix-expression -> postfix-expression postfix-operator
    // postfix-expression -> function-call-expression
    // postfix-expression -> subscript-expression
    Node::Ptr tryParsePostfixExpr(Node::Ptr postfixExpr = nullptr);
    // accept an PostfixExpr and try parse postfix-operator , and return the PostfixExpr;
    Node::Ptr tryParsePostfixExprWithPostfixOperator(Node::Ptr postfixExpr);
    // try parse subscript expr
    Node::Ptr tryParseSubscriptExpr(Node::Ptr postfixExpr);
    
    ArguCallExpr::Ptr tryParseArguCallExpr();
    
    Node::Ptr tryParseFuncCallExpr(Node::Ptr pos·0tfixExpr);
    Node::Ptr tryParseMemberAccessExpr(Node::Ptr postfixExpr);

    Node::Ptr tryParseBinaryExpr();

    // primary-expression -> identifier
    // primary-expression -> literal-expression
    // primary-expression -> parenthesized-expression
    Node::Ptr tryParsePrimaryExpr();

    Node::Ptr tryParseSelfExpr();
    
    // literal-expression -> literal
    // literal-expression -> array-literal | dictionary-literal
    // array-literal -> [ array-literal-items /opts/ ]
    // array-literal-items -> array-literal-item , /opt/ |  array-literal-item, array-literal-items
    // dictionary-literal -> [ dictionary-literal-items ]  | [ : ]
    // dictionary-literal-items -> dictionary-literal-item, /opt/ | dictionary-literal-item, dictionary-literal-items
    // dictionary-literal-item -> expression: expression
    Node::Ptr tryParseLiteralExpr();
    Node::Ptr tryParseArrayOrDictLiteralExpr();
    Node::Ptr tryParseDictItemPairExpr();

    // parenthesized-expression -> `(` expression `)`
    Node::Ptr tryParseParenthesizedExpr();
    
    OperatorExpr::Ptr tryParseOperatorExpr();
    OperatorExpr::Ptr tryParsePostfixOperatorExpr();
    
    // literal -> numeric-literal | string-literal | boolean-literal | nil-literal
    Token::Ptr tryParseLiteral();
    //
    IdentifierExpr::Ptr tryParseIdentifierExpr();


    ////////////////////////////////////////////////////////////////////////////
    // Helper functions
    ////////////////////////////////////////////////////////////////////////////
    Token::Ptr tryEat(TokenKind kind, const std::string &value);

    Token::Ptr tryEat(TokenKind kind);

    void previous();

    Token::Ptr curToken() const;

private:
    const std::vector<Token::Ptr> &tokens;
    std::vector<Token::Ptr>::const_iterator iterator;
    std::vector<Token::Ptr>::const_iterator endIterator;
};

#endif