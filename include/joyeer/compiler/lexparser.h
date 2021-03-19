#ifndef __joyeer_compiler_lexer_lexparser_h__
#define __joyeer_compiler_lexer_lexparser_h__


#include <string>
#include <vector>
#include "token.h"

class LexParser {
public:
  void parse(const std::wstring& content);
    
private:
    void parseStringIdentifier();
    void parseStringLiteral();
    void parseOctalLiteral(std::wstring::const_iterator startAt);
    void parseNumberLiteral(std::wstring::const_iterator startAt);
    void parseHexLiteral(std::wstring::const_iterator startAt);
    void pushOperator(TokenKind kind, std::wstring op, std::wstring::const_iterator startIterator);
    void parseOperator(std::wstring::const_iterator startIterator);
    void parsePunctuation(std::wstring::const_iterator startIterator);

public:
  std::vector<Token::Ptr> tokens;

private:
  std::wstring content;
  std::wstring::const_iterator iterator;
  std::wstring::const_iterator endIterator;

  size_t lineNumber = 0;
  std::wstring::const_iterator lineStartAtPosition;
};


#endif
