#include "token.h"
#include <unordered_map>

Token::Token(TokenKind kind, const std::wstring& rawValue, size_t lineNumber, size_t columnAt):
  kind(kind),
  rawValue(rawValue),
  lineNumber(lineNumber),
  columnAt(columnAt) {

}

std::unordered_set<std::wstring> initKeywordMap();

const std::wstring Keywords::CLASS = L"class";
const std::wstring Keywords::FUNC = L"func";
const std::wstring Keywords::STRUCT = L"struct";
const std::wstring Keywords::VAR = L"var";
const std::wstring Keywords::LET = L"let";
const std::wstring Keywords::IF = L"if";
const std::wstring Keywords::ELSE = L"else";
const std::wstring Keywords::FOR = L"for";
const std::wstring Keywords::WHILE = L"while";
const std::wstring Keywords::IMPORT = L"import";
const std::wstring Keywords::TRY = L"try";
const std::wstring Keywords::IN = L"in";
const std::wstring Keywords::INIT = L"init";
const std::wstring Keywords::SELF = L"self";

const std::unordered_set<std::wstring> Keywords::map = initKeywordMap();

std::unordered_set<std::wstring> initKeywordMap() {
    std::unordered_set<std::wstring> map;
    map.insert(Keywords::CLASS);
    map.insert(Keywords::FUNC);
    map.insert(Keywords::STRUCT);
    map.insert(Keywords::VAR);
    map.insert(Keywords::LET);
    map.insert(Keywords::IF);
    map.insert(Keywords::ELSE);
    map.insert(Keywords::FOR);
    map.insert(Keywords::WHILE);
    map.insert(Keywords::TRY);
    map.insert(Keywords::IN);
    map.insert(Keywords::INIT);
    map.insert(Keywords::SELF);

    return map;
}

bool isKeyword(const std::wstring& keyword) {
  return Keywords::map.find(keyword) != Keywords::map.end();
}

/// Punctuation
const std::wstring Punctuations::OPEN_CURLY_BRACKET = L"{";
const std::wstring Punctuations::CLOSE_CURLY_BRACKET = L"}";
const std::wstring Punctuations::OPEN_ROUND_BRACKET = L"(";
const std::wstring Punctuations::CLOSE_ROUND_BRACKET = L")";
const std::wstring Punctuations::OPEN_SQUARE_BRACKET = L"[";
const std::wstring Punctuations::CLOSE_SQUARE_BRACKET = L"]";
const std::wstring Punctuations::COLON = L":";
const std::wstring Punctuations::COMMA = L",";
const std::wstring Punctuations::DOT = L".";


// Operators
const std::wstring Operators::EQULAS = L"=";
const std::wstring Operators::EQUAL_EQUAL = L"==";
const std::wstring Operators::QUESTION = L"?";
const std::wstring Operators::POINT = L"!";
const std::wstring Operators::PLUS = L"+";
const std::wstring Operators::MINUS = L"-";
const std::wstring Operators::MULTIPLY = L"*";
const std::wstring Operators::DIV = L"/";
const std::wstring Operators::PERCENTAGE = L"%";

std::unordered_map<std::wstring, OperatorPriority> initOperatorPriorities() {
    std::unordered_map<std::wstring, OperatorPriority> result {
        {
            Operators::EQULAS, OperatorPriority::high
        },
        {
            Operators::EQUAL_EQUAL, OperatorPriority::high
        },
        {
            Operators::QUESTION, OperatorPriority::high
        },
        {
            Operators::POINT, OperatorPriority::high
        },
        {
            Operators::PLUS, OperatorPriority::low
        },
        {
            Operators::MINUS, OperatorPriority::low
        },
        {
            Operators::MULTIPLY, OperatorPriority::high
        },
        {
            Operators::DIV, OperatorPriority::high
        },
        {
            Operators::PERCENTAGE, OperatorPriority::high
        },
    };
    return result;
}

const std::unordered_map<std::wstring, OperatorPriority> Operators::prioprityMap = initOperatorPriorities();

OperatorPriority Operators::getPriority(const std::wstring& op) {
    return prioprityMap.find(op)->second;
}


// Literals
const std::wstring Literals::FALSE = L"false";
const std::wstring Literals::TRUE = L"true";
const std::wstring Literals::NIL = L"nil";

