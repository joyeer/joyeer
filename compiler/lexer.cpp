#include "lexer.h"
#include <sstream>
#include <fstream>
#include <codecvt>
#include <string>
#include <iostream>

LexParser::LexParser(const std::string& filepath):
  curLineNumber(0) {
  open(filepath);
}

void LexParser::parse() {
  iterator = content.begin();
  endIterator = content.end();

  switch (*iterator)
  {
  case '\u000A':
    curLineNumber ++;
    lineStartAtPosition = iterator;
    break;
  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    break;
  default:
    break;
  }
}

void LexParser::open(const std::string& filename) {
  std::wifstream wif(filename);

  std::wstringstream wss;
  wss << wif.rdbuf();
  this->content = wss.str();
}

