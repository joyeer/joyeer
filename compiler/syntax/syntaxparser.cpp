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
  if(tryEat(TokenKind::keyword, Keyword::LET) == nullptr) {
    return;
  }

  std::shared_ptr<Token> identifier = tryParseIdentifier();
  if(identifier == nullptr) {
    return ; //Error 
  }

  if(tryEat(TokenKind::operators, Operator::EQULAS) != nullptr) {
    
  }
}

void SyntaxParser::tryParseType() {

}

std::shared_ptr<Token> SyntaxParser::tryParseIdentifier() {
  return tryEat(TokenKind::identifier);
}

void SyntaxParser::tryParseClassDecl() {
  if(tryEat(TokenKind::keyword, Keyword::CLASS) == nullptr) {
    return ;
  }

  std::shared_ptr<Token> className = tryEat(TokenKind::identifier);
  if(className == nullptr) {
    return; // TODO: Error
  }

  if(tryEat(TokenKind::punctuation, Punctuation::OPEN_CURLY_BRACKET) == nullptr) {
    return; // TODO: Error
  }

  if(tryEat(TokenKind::punctuation, Punctuation::CLOSE_CURLY_BRACKET) == nullptr) {
    return ; // TODO: Error
  }
}

std::shared_ptr<Token> SyntaxParser::tryEat(TokenKind kind) {

  if(iterator != endIterator) {
    std::shared_ptr<Token> pToken = curToken();
    if(pToken->kind == kind) {
      return pToken;
    }
  }

  return nullptr;
}

std::shared_ptr<Token> SyntaxParser::tryEat(TokenKind kind, const std::wstring& value) {

  std::shared_ptr<Token> pToken = tryEat(kind);
  if(pToken != nullptr && pToken->value == value) {
    return pToken;
  }

  return nullptr;
}

std::shared_ptr<Token> SyntaxParser::curToken() const{
  return *iterator;
}

