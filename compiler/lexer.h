#ifndef __joyeer_compiler_lexer_h__
#define __joyeer_compiler_lexer_h__

#include <string>

class LexParser {

public:
  LexParser(const std::string& filepath);
  
  void parse();

private:
  void open(const std::string& filepath);

private:
  std::wstring content;
  std::wstring::const_iterator iterator;
  std::wstring::const_iterator endIterator;
  size_t curLineNumber;
  std::wstring::const_iterator lineStartAtPosition;
};

#endif