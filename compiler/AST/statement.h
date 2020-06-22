#ifndef __compiler_ast_statement_h__
#define __compiler_ast_statement_h__

#include "compiler/lexer/lexer.h"

class Node;

struct SourceBlock: Node {
    typedef std::shared_ptr<SourceBlock> Pointer;
    std::vector<std::shared_ptr<Node>> statements;

    SourceBlock(std::vector<std::shared_ptr<Node>> statements);
};

struct CodeBlock: Node {
    typedef std::shared_ptr<CodeBlock> Pointer;
    
    std::vector<std::shared_ptr<Node>> statements;

    CodeBlock(std::vector<std::shared_ptr<Node>> statements);

};

struct ForInStatement: Node {
    typedef std::shared_ptr<ForInStatement> Pointer;
    
    std::shared_ptr<Node> pattern;
    std::shared_ptr<Node> inExpr;
    std::shared_ptr<Node> codeBlock;

    ForInStatement(std::shared_ptr<Node> pattern, std::shared_ptr<Node> inExpr, std::shared_ptr<Node> codeBlock);
};

struct IfStatement: Node {
    typedef std::shared_ptr<IfStatement> Pointer;
    
    std::shared_ptr<Node> condition;
    std::shared_ptr<Node> ifCodeBlock;
    std::shared_ptr<Node> elseCodeBlock;

    IfStatement(std::shared_ptr<Node> condition, std::shared_ptr<Node> ifCodeBlock, std::shared_ptr<Node> elseCodeBlock);
};


#endif
