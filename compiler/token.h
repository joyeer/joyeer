#ifndef __joyeer_compiler_lexer_token_h__
#define __joyeer_compiler_lexer_token_h__

#include <string>
#include <unordered_set>
#include <memory>
#include <vector>

enum TokenKind {
  identifier,
  keyword,
  punctuation,
  operators,

  booleanLiteral,
  nilLiteral,
  floatLiteral,
  decimalLiteral,
  stringLiteral
};

struct Token {
public:
    typedef std::shared_ptr<Token> Pointer;
    
public:
    TokenKind kind;
    std::wstring value;

    int lineNumber;
    int columnAt;

    Token(TokenKind kind, const std::wstring& value, size_t lineNumber, size_t columnAt);

};

struct Keywords {
    static const std::wstring FUNC;
    static const std::wstring CLASS;
    static const std::wstring STRUCT;
    static const std::wstring VAR;
    static const std::wstring LET;
    static const std::wstring IF;
    static const std::wstring ELSE;
    static const std::wstring FOR;
    static const std::wstring WHILE;
    static const std::wstring IMPORT;
    static const std::wstring TRY;
    static const std::wstring IN;
    static const std::wstring INIT;
    static const std::wstring SELF;

    static const std::unordered_set<std::wstring> map;
};

bool isKeyword(const std::wstring& keyword);

struct Punctuations {
    static const std::wstring OPEN_CURLY_BRACKET;   // {
    static const std::wstring CLOSE_CURLY_BRACKET;  // }
    static const std::wstring OPEN_ROUND_BRACKET;   // (
    static const std::wstring CLOSE_ROUND_BRACKET;  // )
    static const std::wstring OPEN_SQUARE_BRACKET;  // [
    static const std::wstring CLOSE_SQUARE_BRACKET; // ]
    static const std::wstring COLON;                // :
    static const std::wstring COMMA;                // ,
    static const std::wstring DOT;                  // .
};

struct Operators {
    static const std::wstring EQULAS;               // =
    static const std::wstring EQUAL_EQUAL;          // ==
    static const std::wstring QUESTION;             // ?
    static const std::wstring POINT;                // !
};

struct Literals {
  static const std::wstring TRUE;
  static const std::wstring FALSE;
  static const std::wstring NIL;
};

#endif
