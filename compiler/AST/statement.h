#ifndef __compiler_ast_statement_h__
#define __compiler_ast_statement_h__

#include <vector>
#include <memory>

struct Decl;
struct Expr;

struct SourceFile: Node {
  std::vector<std::shared_ptr<Decl*>> decls;
  std::vector<std::shared_ptr<Expr*>> exprs;
};


struct ConstantDecl: Node {
    std::shared_ptr<Token> identifier;
    std::shared_ptr<Node> initializer;

    ConstantDecl(std::shared_ptr<Token> identifier, std::shared_ptr<Node> initializer);
};

#endif
