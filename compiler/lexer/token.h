#ifndef __joyeer_compiler_lexer_token_h__
#define __joyeer_compiler_lexer_token_h__

#include <string>

enum TokenKind {
  identifier,
  keyword,
  punctuation,
  operators,

  booleanLiteral,
  nilLiteral,
  floatLiteral,
  decimalLiteral,
  stringLiteral
};

struct Token {
    std::shared_ptr<Token> Pointer;
    TokenKind kind;
    std::wstring value;

    int lineNumber;
    int columnAt;

    Token(TokenKind kind, const std::wstring& value, size_t lineNumber, size_t columnAt);

};

#endif
