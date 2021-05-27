#ifndef __joyeer_compiler_node_types_h__
#define __joyeer_compiler_node_types_h__

#include "joyeer/compiler/node.h"
#include "joyeer/compiler/node+stmt.h"

struct DeclNode : public Node {
    using Ptr = std::shared_ptr<DeclNode>;
public:
    Descriptor::Ptr descriptor = nullptr;
    DeclNode(SyntaxKind kind): Node(kind) {}
};


// Represent a Function in Ast tree.
struct FuncDecl: public DeclNode {
public:
    using Ptr = std::shared_ptr<FuncDecl>;
    
    Node::Ptr identifier = nullptr;               //IdentifierExpr
    Node::Ptr parameterClause;
    Node::Ptr codeBlock;
    Node::Ptr returnType = nullptr;
    bool isConstructor = false;
    bool isStatic = false;

    FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock);
    
    // make FuncDecl as Constructor
    static Ptr makeConstructor(Node::Ptr parameterClause, StmtsBlock::Ptr stmts) {
        auto decl = std::make_shared<FuncDecl>(nullptr, parameterClause, nullptr, stmts);
        decl->isConstructor = true;
        return decl;
    }
    
    virtual std::string getTypeName();
    
    // return func name
    virtual std::string queryName();
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(identifier, NODE_UPDATE_ACTION_SET_PARENT_THIS(identifier))
        NODE_RECURSIVE_UPDATE(parameterClause, NODE_UPDATE_ACTION_SET_PARENT_THIS(parameterClause))
        NODE_RECURSIVE_UPDATE(codeBlock, NODE_UPDATE_ACTION_SET_PARENT_THIS(codeBlock))
        NODE_RECURSIVE_UPDATE(returnType, NODE_UPDATE_ACTION_SET_PARENT_THIS(returnType))
    }
};


struct ClassDecl: public DeclNode {
    using Ptr = std::shared_ptr<ClassDecl>;
    
    Token::Ptr name = nullptr;
    std::vector<Node::Ptr> members;
    
    std::vector<DeclNode::Ptr> staticFields;
    std::vector<DeclNode::Ptr> instanceFields;
    std::vector<FuncDecl::Ptr> staticMethods;
    std::vector<FuncDecl::Ptr> instanceMethods;
    std::vector<FuncDecl::Ptr> constructors;
    
    FuncDecl::Ptr staticConstructor;
    
    ClassDecl(Token::Ptr name, std::vector<Node::Ptr> members);
    
    std::string queryName();
    
    virtual void recursiveUpdate() {
        for(auto& member: members) {
            NODE_RECURSIVE_UPDATE(member, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(member))
        }
        for(auto& field: staticFields) {
            NODE_RECURSIVE_UPDATE(field, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(field))
        }
        for(auto& field: instanceFields) {
            NODE_RECURSIVE_UPDATE(field, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(field))
        }
        for(auto& method: staticMethods) {
            NODE_RECURSIVE_UPDATE(method, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(method))
        }
        for(auto& method: instanceMethods) {
            NODE_RECURSIVE_UPDATE(method, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(method))
        }
        for(auto& constructor: constructors) {
            NODE_RECURSIVE_UPDATE(constructor, NODE_UPDATE_ACTION_SET_PARENT_THIS_2(constructor))
        }
        NODE_RECURSIVE_UPDATE(staticConstructor, NODE_UPDATE_ACTION_SET_PARENT_THIS(staticConstructor))
    }
};

// Reprensent an FileModule in Ast tree, each xxx.joyeer file is a file module
class FileModuleDecl: public ClassDecl {
public:
    using Ptr = std::shared_ptr<FileModuleDecl>;

public:
    FileModuleDecl(FileModuleDescriptor::Ptr descriptor, StmtsBlock::Ptr block);
    
    StmtsBlock::Ptr block;
    
    std::string filename;
    
    virtual std::string queryName();
    // get the top level declarations
    std::vector<Node::Ptr> getTopLevelDecls();
    
    virtual void recursiveUpdate() {
        ClassDecl::recursiveUpdate();
        NODE_RECURSIVE_UPDATE(block, NODE_UPDATE_ACTION_SET_PARENT_THIS(block))
    }

};

// `let` or `var` declaration
struct VarDecl: public DeclNode {
    using Ptr = std::shared_ptr<VarDecl>;
    
    // `let` mutable = false, `var` mutable = true
    bool _mutable = true;
    Pattern::Ptr pattern;
    Node::Ptr initializer;
    Descriptor::Ptr parentDescriptor;
    
    VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer);
    
    virtual void recursiveUpdate() {
        NODE_RECURSIVE_UPDATE(pattern, NODE_UPDATE_ACTION_SET_PARENT_THIS(pattern))
        NODE_RECURSIVE_UPDATE(initializer, NODE_UPDATE_ACTION_SET_PARENT_THIS(initializer))
    }
    
};

#endif
