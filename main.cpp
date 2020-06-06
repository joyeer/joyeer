#include <iostream>
#include "compiler/lexer.h"

int main(int argc, char** argv) {
  if(argc < 2) {
    std::cout << "input file";
    return -1;
  }

  std::string filepath = std::string(argv[1]);
  LexParser parser(filepath);
  parser.parse();

  return 0;
}