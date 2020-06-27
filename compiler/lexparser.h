#ifndef __joyeer_compiler_lexer_lexparser_h__
#define __joyeer_compiler_lexer_lexparser_h__


#include <string>
#include <vector>
#include "token.h"

class LexParser {

public:
  LexParser(const std::string& filepath);
  
  void parse();

private:
    void open(const std::string& filepath);

    void parseStringIdentifier();
    
    void parseStringLiteral();
    void parseOctalLiteral(std::wstring::const_iterator startAt);
    void parseNumberLiteral(std::wstring::const_iterator startAt);
    void parseHexLiteral(std::wstring::const_iterator startAt);
    void pushOperator(TokenKind kind, std::wstring op, std::wstring::const_iterator startIterator);
    void parseOperator(std::wstring::const_iterator startIterator);
    void parsePunctuation(std::wstring::const_iterator startIterator);

public:
  std::vector<std::shared_ptr<Token>> tokens;

private:
  std::wstring content;
  std::wstring::const_iterator iterator;
  std::wstring::const_iterator endIterator;

  size_t lineNumber;
  std::wstring::const_iterator lineStartAtPosition;
};


#endif
