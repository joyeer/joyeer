#ifndef __joyeer_compiler_node_stmt_h__
#define __joyeer_compiler_node_stmt_h__

#include <utility>

#include "joyeer/compiler/node.h"

struct FileImportStmt: Node {
    using Ptr = std::shared_ptr<FileImportStmt>;
    
    Token::Ptr stringLiteral;
    JrModuleClass* moduleClass;
    
    explicit FileImportStmt(Token::Ptr stringLiteral);
    
    const std::string getImportedFilename();
    
    void recursiveUpdate() override { }
};

// StmtsBlock represent an { ... } code block
struct StmtsBlock: Node {
    using Ptr = std::shared_ptr<StmtsBlock>;
    
    std::vector<Node::Ptr> statements;

    explicit StmtsBlock(std::vector<Node::Ptr> statements):
        Node(SyntaxKind::stmtsBlock),
        statements(std::move(statements)) {
            symtable = std::make_shared<SymbolTable>();
    }
    
    void recursiveUpdate() override {
        for(auto& statement: statements) {
            NODE_RECURSIVE_UPDATE(statement, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(statement))
        }
    }
};

// For In statement
struct ForInStmt: Node {
    using Ptr = std::shared_ptr<ForInStmt>;
    
    Node::Ptr pattern;
    Node::Ptr inExpr;
    Node::Ptr codeBlock;

    ForInStmt(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock);
    
    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(pattern, NODE_UPDATE_ACTION_SET_PARENT_THIS(pattern))
        NODE_RECURSIVE_UPDATE(inExpr, NODE_UPDATE_ACTION_SET_PARENT_THIS(inExpr))
        NODE_RECURSIVE_UPDATE(codeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(codeBlock))
    }
};

struct WhileStmt: Node {
    using Ptr = std::shared_ptr<WhileStmt>;
    
    Node::Ptr expr;
    Node::Ptr codeBlock;
    
    WhileStmt(Node::Ptr expr, Node::Ptr codeBlock);
    
    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
        NODE_RECURSIVE_UPDATE(codeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(codeBlock))
    }
};

struct IfStmt: Node {
    using Ptr = std::shared_ptr<IfStmt>;
    
    Node::Ptr condition;
    Node::Ptr ifCodeBlock;
    Node::Ptr elseCodeBlock;

    IfStmt(Node::Ptr condition, Node::Ptr ifCodeBlock, Node::Ptr elseCodeBlock);
    
    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(condition, NODE_UPDATE_ACTION_SET_PARENT_THIS(condition))
        NODE_RECURSIVE_UPDATE(ifCodeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(ifCodeBlock))
        NODE_RECURSIVE_UPDATE(elseCodeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(elseCodeBlock))
    }
};

struct ReturnStmt: Node {
    using Ptr = std::shared_ptr<ReturnStmt>;
    
    Node::Ptr expr;
    
    explicit ReturnStmt(Node::Ptr expr);
    
    void recursiveUpdate() override {
        NODE_RECURSIVE_UPDATE(expr, NODE_UPDATE_ACTION_SET_PARENT_THIS(expr))
    }
};


#endif
