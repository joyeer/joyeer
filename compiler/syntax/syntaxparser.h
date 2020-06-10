#ifndef __joyeer_syntax_syntaxparser_h__
#define __joyeer_syntax_syntaxparser_h__

#include "compiler/AST/ast.h"
#include "compiler/lexer/lexer.h"

class SyntaxParser {
public:
  SyntaxParser(const std::vector<std::shared_ptr<Token>>& tokens);
  std::shared_ptr<SourceFile> parse();

private:
  void parseDecl();  

  void tryParseClassDecl();

  // Parse the constant declaration, e.g. let constValue = "10"
  void tryParseConstDecl();
  // Parse the variable declaration, e.g. var varValue = "10"
  void tryParseVarDecl();

  void tryParseExpr();
  void tryParsePrefixExpr();
  void tryParsePostfixExpr();
  void tryParseBinaryExpr();

  void tryParsePrimaryExpr();
  void tryParseLiteralExpr();
  
  
  void tryParseBinaryOperator();
  void tryParseConditionalOperator();
  std::shared_ptr<Node> tryParseAssignmentOperator();
  std::shared_ptr<Token> tryParseOperator();
  
  void tryParseType();
  std::shared_ptr<Token> tryParseIdentifier();

  std::shared_ptr<Token> tryEat(TokenKind kind, const std::wstring& value);
  std::shared_ptr<Token> tryEat(TokenKind kind);

  std::shared_ptr<Token> curToken() const;

private:
  const std::vector<std::shared_ptr<Token>>& tokens;
  std::vector<std::shared_ptr<Token>>::const_iterator iterator;
  std::vector<std::shared_ptr<Token>>::const_iterator endIterator;
};


#endif
