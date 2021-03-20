#include "joyeer/compiler/lexparser.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/runtime/buildin.h"
#include <codecvt>
#include <string>


void LexParser::parse(const std::wstring& content) {
    this->content = content;
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
                parseStringIdentifier();
                break;
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                parseNumberLiteral(iterator - 1);
                break;
            case '0':
                if (iterator == endIterator) {
                    auto token = std::make_shared<Token>(TokenKind::decimalLiteral, L"0", lineNumber, iterator - lineStartAtPosition);
                    token->intValue = 0;
                    tokens.push_back(token);
                    break;
                }
                switch (*iterator) {
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                        break;
                        parseOctalLiteral(iterator);
                    case '8': case '9':
                        Diagnostics::reportError(L"Octal number only contains 0,1,2,3,4,5,6,7");
                        break;
                    default:
                        auto token = std::make_shared<Token>(TokenKind::decimalLiteral, L"0", lineNumber, iterator - lineStartAtPosition);
                        token->intValue = 0;
                        tokens.push_back(token);
                        break;
                }
                
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
                if(iterator != endIterator) {
                    if(*iterator == '=') {
                        iterator ++;
                        pushOperator(TokenKind::operators, Operators::NOT_EQUALS, iterator);
                        continue;
                    }
                }
                parseOperator(iterator - 1);
                break;
            case '*':
                parseOperator(iterator - 1);
                break;
            case '%':
                parseOperator(iterator - 1);
                break;
            case '<':
                if(iterator != endIterator) {
                    if(*iterator == '=') {
                        iterator ++;
                        pushOperator(TokenKind::operators, Operators::LESS_EQ, iterator);
                        continue;
                    }
                }
                parseOperator(iterator - 1);
                break;
            case '>':
                if(iterator != endIterator) {
                    if(*iterator == '=') {
                        iterator ++;
                        pushOperator(TokenKind::operators, Operators::GERATER_EQ, iterator);
                        continue;
                    }
                }
                parseOperator(iterator - 1);
                break;
            case '&':
                if (iterator != endIterator) {
                    if (*iterator == '&') {
                        iterator ++;
                        pushOperator(TokenKind::operators, Operators::AND_AND, iterator);
                        continue;
                    }
                }
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
            case ';':
                parsePunctuation(iterator - 1);
                break;
            case '\"':
                parseStringLiteral();
                break;
            default:
                break;
            }
  }
}

void LexParser::parseOctalLiteral(std::wstring::const_iterator startAt) {
    while (iterator < endIterator) {
        switch (*iterator) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                break;
            case '8': case '9':
                Diagnostics::reportError(L"Octal number only contains 0,1,2,3,4,5,6,7");
                goto label;
            default:
                goto label;
        }
    }
    
label:
    std::wstring number(startAt, iterator);
    auto token = std::make_shared<Token>(TokenKind::decimalLiteral, number, lineNumber, startAt - lineStartAtPosition);
    tokens.push_back(token);
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

void LexParser::parseStringIdentifier() {
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

void LexParser::parseStringLiteral() {
    auto startAt = iterator;
    while(iterator != endIterator) {
        if(*iterator == '\\') {
            iterator ++;
            if(iterator == endIterator) {
                Diagnostics::reportError(L"[Error] except character after \"");
                return;
            }
        } else if (*iterator == '\"') {
            iterator ++ ;
            break;
        }
        iterator ++;
    }
    
    const std::wstring identifier(startAt, iterator - 1);
    auto stringLiteral = std::make_shared<Token>(TokenKind::stringLiteral, identifier, lineNumber, iterator - startAt);
    Global::strings.push_back(identifier);
    stringLiteral->index = Global::strings.size() - 1;
    tokens.push_back(stringLiteral);
}
