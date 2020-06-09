#include "syntax.h"
#include "../AST/ast.h"

SyntaxParser::SyntaxParser(const std::vector<Token>& tokens): 
  tokens(tokens) {
  iterator = tokens.begin();
  endIterator = tokens.end();
}

std::shared_ptr<SourceFile> SyntaxParser::parse() {
  return std::shared_ptr<SourceFile>(new SourceFile());
}

void SyntaxParser::parseDecl() {
  
}

void SyntaxParser::tryParseConstDecl() {

}

void SyntaxParser::tryParseType() {
  
}

bool SyntaxParser::tryEat(TokenKind kind, const std::wstring& value) {
  if(iterator->kind == kind && iterator->value == value) {
    iterator ++;
    return true;
  }

  return false;
}

