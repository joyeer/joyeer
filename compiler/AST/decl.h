#ifndef __compiler_ast_decl_h__
#define __compiler_ast_decl_h__

#include "node.h"
#include "compiler/lexer/lexer.h"

struct Decl : Node {

};

struct ConstantDecl: Node {
    std::shared_ptr<Token> identifier;
    std::shared_ptr<Node> initializer;

    ConstantDecl(std::shared_ptr<Token> identifier, std::shared_ptr<Node> initializer);
};

struct VarDecl: Node {
    std::shared_ptr<Token> identifier;
    std::shared_ptr<Node> initializer;

    VarDecl(std::shared_ptr<Token> identifier, std::shared_ptr<Node> initializer);
};

#endif
