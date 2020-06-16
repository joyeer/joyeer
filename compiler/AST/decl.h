#ifndef __compiler_ast_decl_h__
#define __compiler_ast_decl_h__

#include "node.h"
#include "compiler/lexer/lexer.h"


struct Pattern: Node {
    std::shared_ptr<Token> identifier;
    std::shared_ptr<Node> type;
    
    Pattern(std::shared_ptr<Token> identifier, std::shared_ptr<Node> type);
};


struct ConstantDecl: Node {
    std::shared_ptr<Node> pattern;
    std::shared_ptr<Node> initializer;

    ConstantDecl(std::shared_ptr<Node> pattern, std::shared_ptr<Node> initializer);
};

struct VarDecl: Node {
    std::shared_ptr<Node> pattern;
    std::shared_ptr<Node> initializer;

    VarDecl(std::shared_ptr<Node> pattern, std::shared_ptr<Node> initializer);
};

struct ClassDecl: Node {
    std::shared_ptr<Token> name;
    std::vector<std::shared_ptr<Node>> members;

    ClassDecl(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Node>> members);
};

struct ParameterClause: Node {
    std::vector<std::shared_ptr<Node>> parameters;
    
    ParameterClause(std::vector<std::shared_ptr<Node>> parameters);
};

struct FuncDecl: Node {
    std::shared_ptr<Token> name;
    std::shared_ptr<Node> parameterClause;
    std::shared_ptr<Node> codeBlock;

    FuncDecl(std::shared_ptr<Token> name, std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock);
};

struct InitializerDecl: Node {
    std::shared_ptr<Node> parameterClause;
    std::shared_ptr<Node> codeBlock;
    
    InitializerDecl(std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock);
};

#endif
