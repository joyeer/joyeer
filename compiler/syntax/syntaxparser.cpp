#include "syntaxparser.h"

SyntaxParser::SyntaxParser(const std::vector<std::shared_ptr<Token>>& tokens): 
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

void SyntaxParser::tryParseClassDecl() {
  if(tryEat(TokenKind::keyword, Keyword::CLASS) == false) {
    return ;
  }
}

bool SyntaxParser::tryEat(TokenKind kind, const std::wstring& value) {

  std::shared_ptr<Token> token = curToken();
  if(token->kind == kind && token->value == value) {
    iterator ++;
    return true;
  }

  return false;
}

std::shared_ptr<Token> SyntaxParser::curToken() const {
  return *iterator;
}

