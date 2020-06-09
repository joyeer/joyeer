#ifndef __joyeer_compiler_lexer_token_h__
#define __joyeer_compiler_lexer_token_h__

#include <string>

enum TokenKind {
  identifier,
  keyword,
  punctuation,
  operators,
  floatLiteral,
  decimalLiteral
};

struct Token {
  TokenKind kind;
  std::wstring value;
  
  size_t lineNumber;
  size_t columnAt;

  Token(TokenKind kind, const std::wstring& value, size_t lineNumber, size_t columnAt);

};

#endif
