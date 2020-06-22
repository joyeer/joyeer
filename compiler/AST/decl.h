#ifndef __compiler_ast_decl_h__
#define __compiler_ast_decl_h__

#include "node.h"
#include "compiler/lexer/lexer.h"
#include "expression.h"

struct Pattern: Node {
    typedef std::shared_ptr<Pattern> Pointer;
    
    std::shared_ptr<Token> identifier;
    Type::Pointer type;
    
    Pattern(std::shared_ptr<Token> identifier, Type::Pointer type);
    
};

struct ConstDecl: Node {
    typedef std::shared_ptr<ConstDecl> Pointer;
    
    std::shared_ptr<Node> pattern;
    std::shared_ptr<Node> initializer;

    ConstDecl(std::shared_ptr<Node> pattern, std::shared_ptr<Node> initializer);
};

struct VarDecl: public Node {
    typedef std::shared_ptr<VarDecl> Pointer;
    
    Pattern::Pointer pattern;
    std::shared_ptr<Node> initializer;

    VarDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer);
};

struct ClassDecl: Node {
    typedef std::shared_ptr<ClassDecl> Pointer;
    
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

struct ConstructorDecl: Node {
    typedef std::shared_ptr<ConstructorDecl> Pointer;
    
    std::shared_ptr<Node> parameterClause;
    std::shared_ptr<Node> codeBlock;
    
    ConstructorDecl(std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock);
};

#endif
