#ifndef __compiler_ast_decl_h__
#define __compiler_ast_decl_h__

#include <memory>

struct Decl : Node {

};

struct VarDecl: Decl {
  std::shared_ptr<Token> identifier;
};

struct ConstDecl: Decl {
  std::shared_ptr<Token> identifier;

}


#endif
