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
  void tryParseConstDecl();
  void tryParseVarDecl();

  void tryParseClassDecl();
  void tryParseClassName();

  void tryParseType();

  bool tryEat(TokenKind kind, const std::wstring& value);
  const Token& tryEatAndReturn(TokenKind kind);

  std::shared_ptr<Token> curToken() const;

private:
  const std::vector<std::shared_ptr<Token>>& tokens;
  std::vector<std::shared_ptr<Token>>::const_iterator iterator;
  std::vector<std::shared_ptr<Token>>::const_iterator endIterator;
};


#endif
