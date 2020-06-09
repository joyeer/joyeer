#ifndef __joyeer_compiler_syntax_syntax_h__
#define __joyeer_compiler_syntax_syntax_h__

#include "../token.h"
#include "../AST/ast.h"
#include <vector>

class SyntaxParser {
public:
  SyntaxParser(const std::vector<Token>& tokens);
  std::shared_ptr<SourceFile> parse();

private:
  void parseDecl();
  void tryParseConstDecl();
  void tryParseVarDecl();
  void tryParseClassDecl();

  void tryParseType();

  bool tryEat(TokenKind kind, const std::wstring& value);

private:
  const std::vector<Token>& tokens;
  std::vector<Token>::const_iterator iterator;
  std::vector<Token>::const_iterator endIterator;
};

#endif