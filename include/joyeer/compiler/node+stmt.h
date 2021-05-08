#ifndef __joyeer_compiler_node_stmt_h__
#define __joyeer_compiler_node_stmt_h__

#include "joyeer/compiler/node.h"

struct FileImportStmt: Node {
    typedef std::shared_ptr<FileImportStmt> Ptr;
    
    Token::Ptr stringLiteral;
    JrModuleClass* moduleClass;
    
    FileImportStmt(Token::Ptr stringLiteral);
    
    const std::string getImportedFilename();
    
};

// StmtsBlock represent an { ... } code block
struct StmtsBlock: Node {
    typedef std::shared_ptr<StmtsBlock> Ptr;
    
    std::vector<Node::Ptr> statements;

    StmtsBlock(std::vector<Node::Ptr> statements);

};

// For In statement
struct ForInStmt: Node {
    typedef std::shared_ptr<ForInStmt> Ptr;
    
    Node::Ptr pattern;
    Node::Ptr inExpr;
    Node::Ptr codeBlock;

    ForInStmt(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock);
};

struct WhileStmt: Node {
    typedef std::shared_ptr<WhileStmt> Ptr;
    
    Node::Ptr expr;
    Node::Ptr codeBlock;
    
    WhileStmt(Node::Ptr expr, Node::Ptr codeBlock);
};

struct IfStmt: Node {
    typedef std::shared_ptr<IfStmt> Ptr;
    
    Node::Ptr condition;
    Node::Ptr ifCodeBlock;
    Node::Ptr elseCodeBlock;

    IfStmt(Node::Ptr condition, Node::Ptr ifCodeBlock, Node::Ptr elseCodeBlock);
};

struct ReturnStmt: Node {
    typedef std::shared_ptr<ReturnStmt> Ptr;
    
    Node::Ptr expr;
    
    ReturnStmt(Node::Ptr expr);
};


#endif
