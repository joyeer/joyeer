#include "token.h"

Token::Token(TokenKind kind, const std::wstring& value, size_t lineNumber, size_t columnAt):
  kind(kind),
  value(value), 
  lineNumber(lineNumber),
  columnAt(columnAt) {

}