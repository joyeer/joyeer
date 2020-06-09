#include "keyword.h"

std::unordered_set<std::wstring> initKeywordMap();

const std::wstring Keyword::CLASS = L"class";
const std::wstring Keyword::FUNC = L"func";
const std::wstring Keyword::STRUCT = L"struct";
const std::wstring Keyword::VAR = L"var";
const std::wstring Keyword::LET = L"let";
const std::wstring Keyword::IF = L"if";
const std::wstring Keyword::ELSE = L"else";
const std::wstring Keyword::FOR = L"for";
const std::wstring Keyword::WHILE = L"while";
const std::wstring Keyword::IMPORT = L"import";
const std::wstring Keyword::TRY = L"try";

const std::unordered_set<std::wstring> Keyword::map = initKeywordMap();

std::unordered_set<std::wstring> initKeywordMap() {
  std::unordered_set<std::wstring> map;
  map.insert(Keyword::CLASS);
  map.insert(Keyword::FUNC);
  map.insert(Keyword::STRUCT);
  map.insert(Keyword::VAR);
  map.insert(Keyword::LET);
  map.insert(Keyword::IF);
  map.insert(Keyword::ELSE);
  map.insert(Keyword::FOR);
  map.insert(Keyword::WHILE);
  map.insert(Keyword::TRY);

  return map;
}

bool isKeyword(const std::wstring& keyword) {
  return Keyword::map.find(keyword) != Keyword::map.end();
}

/// Punctuation
const std::wstring Punctuation::OPEN_CURLY_BRACKET = L"{";
const std::wstring Punctuation::CLOSE_CURLY_BRACKET = L"}";
const std::wstring Punctuation::OPEN_ROUND_BRACKET = L"(";
const std::wstring Punctuation::CLOSE_ROUND_BRACKET = L")";
const std::wstring Punctuation::OPEN_SQUARE_BRACKET = L"[";
const std::wstring Punctuation::CLOSE_SQUARE_BRACKET = L"]";

// Operators
const std::wstring Operator::EQULAS = L"=";
