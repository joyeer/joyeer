#ifndef __compiler_ast_statement_h__
#define __compiler_ast_statement_h__

#include "compiler/lexer/lexer.h"

class Node;

struct SourceFile: Node {
    std::vector<std::shared_ptr<Node>> decls;

    SourceFile(std::vector<std::shared_ptr<Node>> decls);
};

struct CodeBlock: Node {
    std::vector<std::shared_ptr<Node>> statements;

    CodeBlock(std::vector<std::shared_ptr<Node>> statements);

};

struct ForInStatement: Node {
    std::shared_ptr<Token> pattern;
    std::shared_ptr<Node> inExpr;
    std::shared_ptr<Node> codeBlock;

    ForInStatement(std::shared_ptr<Token> pattern, std::shared_ptr<Node> inExpr, std::shared_ptr<Node> codeBlock);
};

struct IfStatement: Node {
    std::shared_ptr<Node> condition;
    std::shared_ptr<Node> ifCodeBlock;
    std::shared_ptr<Node> elseCodeBlock;

    IfStatement(std::shared_ptr<Node> condition, std::shared_ptr<Node> ifCodeBlock, std::shared_ptr<Node> elseCodeBlock);
};


#endif
