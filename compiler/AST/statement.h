#ifndef __compiler_ast_statement_h__
#define __compiler_ast_statement_h__

#include "compiler/lexer/lexer.h"

class Node;

struct SourceFile: Node {
    std::vector<std::shared_ptr<Node>> decls;
    std::vector<std::shared_ptr<Node>> exprs;
};

struct CodeBlock: Node {
    std::vector<std::shared_ptr<Node>> statements;
    CodeBlock(std::vector<std::shared_ptr<Node>> statements);

};

struct ForInStatement: Node {
    std::shared_ptr<Token> patten;
    std::shared_ptr<Node> inExpr;
    std::shared_ptr<Node> codeBlock;

    ForInStatement(std::shared_ptr<Token> pattern)
};


#endif
