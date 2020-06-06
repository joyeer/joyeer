#ifndef __joyeer_compiler_keyword_h__
#define __joyeer_compiler_keyword_h__

#include <string>
#include <unordered_set>

struct Keyword {
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

  static const std::unordered_set<std::wstring> map;
};

bool isKeyword(const std::wstring& keyword);


#endif
