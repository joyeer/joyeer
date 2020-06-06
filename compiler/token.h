#ifndef __joyeer_compiler_token_h__
#define __joyeer_compiler_token_h__

#include <string>

enum TokenKind {
  identifier,
  keyword
};

struct Token {
  std::wstring value;
};

#endif