#ifndef __joyeer_compiler_node_stmt_h__
#define __joyeer_compiler_node_stmt_h__

#include "joyeer/compiler/node.h"

struct FileImportStmt: Node {
    typedef std::shared_ptr<FileImportStmt> Ptr;
    
    Token::Ptr stringLiteral;
    JrModuleClass* moduleClass;
    
    FileImportStmt(Token::Ptr stringLiteral);
    
    const std::string getImportedFilename();
    
    virtual void recursiveUpdate() { }
};

// StmtsBlock represent an { ... } code block
struct StmtsBlock: Node {
    using Ptr = std::shared_ptr<StmtsBlock>;
    
    std::vector<Node::Ptr> statements;

    StmtsBlock(std::vector<Node::Ptr> statements):
        Node(SyntaxKind::stmtsBlock),
        statements(statements) {
            symtable = std::make_shared<SymbolTable>();
    }
    
    virtual void recursiveUpdate() {
        for(auto& statement: statements) {
            NODE_RECURSIVE_UPDATE(statement, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(statement))
        }
    }
};

// For In statement
struct ForInStmt: Node {
    typedef std::shared_ptr<ForInStmt> Ptr;
    
    Node::Ptr pattern;
    Node::Ptr inExpr;
    Node::Ptr codeBlock;

    ForInStmt(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(pattern, NODE_UPDATE_ACTION_SET_PARENT_THIS(pattern))
        NODE_RECURSIVE_UPDATE(inExpr, NODE_UPDATE_ACTION_SET_PARENT_THIS(inExpr))
        NODE_RECURSIVE_UPDATE(codeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(codeBlock))
    }
};

struct WhileStmt: Node {
    typedef std::shared_ptr<WhileStmt> Ptr;
    
    Node::Ptr expr;
    Node::Ptr codeBlock;
    
    WhileStmt(Node::Ptr expr, Node::Ptr codeBlock);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(codeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(codeBlock))
    }
};

struct IfStmt: Node {
    typedef std::shared_ptr<IfStmt> Ptr;
    
    Node::Ptr condition;
    Node::Ptr ifCodeBlock;
    Node::Ptr elseCodeBlock;

    IfStmt(Node::Ptr condition, Node::Ptr ifCodeBlock, Node::Ptr elseCodeBlock);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(condition, NODE_UPDATE_ACTION_SET_PARENT_THIS(condition))
        NODE_RECURSIVE_UPDATE(ifCodeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(ifCodeBlock))
        NODE_RECURSIVE_UPDATE(elseCodeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(elseCodeBlock))
    }
};

struct ReturnStmt: Node {
    typedef std::shared_ptr<ReturnStmt> Ptr;
    
    Node::Ptr expr;
    
    ReturnStmt(Node::Ptr expr);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
    }
};


#endif
