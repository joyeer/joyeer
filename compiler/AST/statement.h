#ifndef __compiler_ast_statement_h__
#define __compiler_ast_statement_h__

#include "compiler/lexer/lexer.h"

class Node;

struct SourceFile: Node {
  std::vector<std::shared_ptr<Node*>> decls;
  std::vector<std::shared_ptr<Node*>> exprs;
};




#endif
