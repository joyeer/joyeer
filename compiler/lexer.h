#ifndef __joyeer_compiler_lexer_h__
#define __joyeer_compiler_lexer_h__

#include <string>
#include <vector>
#include "token.h"

class LexParser {

public:
  LexParser(const std::string& filepath);
  
  void parse();

private:
  void open(const std::string& filepath);

  void parseStringLiteral();
  
private:
  std::vector<Token> tokens;

  std::wstring content;
  std::wstring::const_iterator iterator;
  std::wstring::const_iterator endIterator;

  size_t lineNumber;
  std::wstring::const_iterator lineStartAtPosition;
};

#endif