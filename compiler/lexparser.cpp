#include "lexparser.h"
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
            case '\u0000':
            case '\u0009':
            case '\u000B':
            case '\u000C':
            case '\u0020':
                break;
            case '\u000A':
                lineNumber ++;
                lineStartAtPosition = (iterator - 1);
                break;
            case '\u000D':
                lineNumber ++;
                lineStartAtPosition = iterator - 1;
                if(iterator != endIterator && *iterator == '\u000A') {
                    iterator ++;
                }
                break;
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            case '_':
                parseStringLiteral();
                break;
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                parseNumberLiteral(iterator - 1);
                break;
            case '/':
                parseOperator(iterator - 1);
                break;
            case '=':
                if (iterator != endIterator) {
                    if (*iterator == '=') {
                        iterator ++;
                        pushOperator(TokenKind::operators, Operators::EQUAL_EQUAL, iterator);
                        continue;
                    }
                }
                parseOperator(iterator - 1);
                break;
            case '-':
                parseOperator(iterator - 1);
                break;
            case '+':
                parseOperator(iterator - 1);
                break;
            case '!':
                parseOperator(iterator - 1);
                break;
            case '*':
                parseOperator(iterator - 1);
                break;
            case '%':
                parseOperator(iterator - 1);
                break;
            case '<':
                parseOperator(iterator - 1);
                break;
            case '>':
                parseOperator(iterator - 1);
                break;
            case '&':
                parseOperator(iterator - 1);
                break;
            case '|':
                parseOperator(iterator - 1);
                break;
            case '^':
                parseOperator(iterator - 1);
                break;
            case '~':
                parseOperator(iterator - 1);
                break;
            case '?':
                parseOperator(iterator - 1);
                break;
            case '(':
                parsePunctuation(iterator - 1);
                break;
            case ')':
                parsePunctuation(iterator - 1);
                break;
            case '{':
                parsePunctuation(iterator - 1);
                break;
            case '}':
                parsePunctuation(iterator - 1);
                break;
            case '[':
                parsePunctuation(iterator - 1);
                break;
            case ']':
                parsePunctuation(iterator - 1);
                break;
            case '.':
                parsePunctuation(iterator - 1);
                break;
            case ',':
                parsePunctuation(iterator - 1);
                break;
            case ':':
                parsePunctuation(iterator - 1);
                break;
            case '@':
                parsePunctuation(iterator - 1);
                break;
            case '#':
                parsePunctuation(iterator - 1);
                break;
            default:
                break;
            }
  }
}

void LexParser::parseNumberLiteral(std::wstring::const_iterator startAt) {

  bool isFloatingLiteral = false;
  while (iterator != endIterator) {
    switch (*iterator ++)
    {
    case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
      break;
    default:
      iterator --;
      goto break_label_1;
    }
  }

  break_label_1:

  bool hasFraction = false;
  if(*iterator == '.') {
    iterator ++ ;
    std::wstring::const_iterator fractionStartIterator = iterator;          
    while(iterator != endIterator ) {
      switch (*iterator ++)
      {
      case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
        hasFraction = true;
        isFloatingLiteral = true;
        break;
      default:
        iterator --;
        break;
      }
    }
    if(!hasFraction) {
      iterator --;
    } 
  }

    std::wstring identifier(startAt, iterator);
    auto token = std::make_shared<Token>(TokenKind::decimalLiteral, identifier, lineNumber, iterator - startAt);
    token->intValue = std::stoi(identifier);
    tokens.push_back(token);
}

void LexParser::parseHexLiteral(std::wstring::const_iterator startAt) {

}

void LexParser::parseOperator(std::wstring::const_iterator startIterator) {
  std::wstring operators(startIterator, startIterator + 1);
  tokens.push_back(
    std::shared_ptr<Token>(new Token(TokenKind::operators, operators, lineNumber, iterator - startIterator))
  );
}

void LexParser::pushOperator(TokenKind kind, std::wstring op, std::wstring::const_iterator startIterator) {
    tokens.push_back(std::shared_ptr<Token>(new Token(TokenKind::operators, op, lineNumber, iterator - startIterator)));
}

void LexParser::parsePunctuation(std::wstring::const_iterator startIterator) {
  std::wstring punctuation(startIterator, startIterator + 1);
  tokens.push_back(
    std::shared_ptr<Token>(new Token(TokenKind::punctuation, punctuation, lineNumber, iterator - startIterator))
  );
}

void LexParser::parseStringLiteral() {
  std::wstring::const_iterator startAt = iterator - 1;
  while(iterator != endIterator) {
    switch (*iterator ++) {
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
  
    std::shared_ptr<Token> token;
    if(isKeyword(identifier)) {
        token = std::shared_ptr<Token>(new Token(TokenKind::keyword, identifier, lineNumber, iterator - startAt));
    } else if(identifier == Literals::NIL) {
        token = std::shared_ptr<Token>(new Token(TokenKind::nilLiteral, identifier, lineNumber, iterator - startAt));
    } else if(identifier == Literals::TRUE || identifier == Literals::FALSE) {
        token = std::shared_ptr<Token>(new Token(TokenKind::booleanLiteral, identifier, lineNumber, iterator - startAt));
    } else {
        token = std::shared_ptr<Token>(new Token(TokenKind::identifier, identifier, lineNumber, iterator - startAt));
    }
    tokens.push_back(token);
}

void LexParser::open(const std::string& filename) {
  std::wifstream wif(filename);

  std::wstringstream wss;
  wss << wif.rdbuf();
  this->content = wss.str();
}
