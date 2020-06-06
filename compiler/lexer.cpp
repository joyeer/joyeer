#include "lexer.h"
#include <sstream>
#include <fstream>
#include <codecvt>
#include <string>
#include <iostream>

LexParser::LexParser(const std::string& filepath):
  lineNumber(0) {
  open(filepath);
}

void LexParser::parse() {
  iterator = content.begin();
  endIterator = content.end();

  while (iterator != endIterator) {
    switch (*iterator ++) {
    case '\u000A':
      lineNumber ++;
      lineStartAtPosition = iterator;
      break;
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case '_':
      parseStringLiteral();
      break;
    default:
      break;
    }
  }
}

void LexParser::parseStringLiteral() {
  std::wstring::const_iterator startAt = iterator - 1;
  while(iterator != endIterator) {
    switch (*iterator ++)
    {
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
    case '_':
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      break; 
    default:
      iterator --;
      goto exit_label;
    }
  }

  exit_label:
  std::wstring identifier(startAt, iterator);
  
  Token token(TokenKind::identifier, identifier, lineNumber, iterator - startAt);
  tokens.push_back(token);
}

void LexParser::open(const std::string& filename) {
  std::wifstream wif(filename);

  std::wstringstream wss;
  wss << wif.rdbuf();
  this->content = wss.str();
}

