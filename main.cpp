#include <iostream>
#include "compiler/lexer/lexer.h"
#include "compiler/syntax/syntax.h"


int main(int argc, char** argv) {
  if(argc < 2) {
    std::cout << "input file";
    return -1;
  }

  std::string filepath = std::string(argv[1]);
  LexParser parser(filepath);
  parser.parse();

  SyntaxParser syntaxParser(parser.tokens);
  syntaxParser.parse();
    SymbolFactoryPtr symFactory = std::make_shared<SymbolFactory>();
    
    Binder binder(symFactory);
  
  return 0;
}
