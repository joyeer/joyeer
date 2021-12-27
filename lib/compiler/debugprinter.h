#ifndef __joyeer_compiler_node_debugprint_h__
#define __joyeer_compiler_node_debugprint_h__

#include "joyeer/compiler/node+visitor.h"
#include <fstream>
#include <sstream>

#define DEBUG_BLOCK_START \
    incTab();             \
    newline();

#define DEBUG_BLOCK_END decTab();

struct YMLPrinter {

    explicit YMLPrinter(const std::string& filename);

    void printTab();
    void incTab();
    void decTab();

    void newline() {
        output << std::endl;
        printTab();
    }

    void close();

    int size = 0;
    std::ofstream output;
};

// ASM node printer
struct NodeDebugPrinter : public NodeVisitor, public YMLPrinter {


    explicit NodeDebugPrinter(const std::string& filename);

    // print AST into debug file
    void print(const Node::Ptr& node) {
        NodeVisitor::visit(node);
    }

    /// escaping c string for debug display
    static std::string escapeString(const std::string &s) {
        std::ostringstream o;
        for (char c : s) {
            switch (c) {
                case '"': o << "\\\""; break;
                case '\\': o << "\\\\"; break;
                case '\b': o << "\\b"; break;
                case '\f': o << "\\f"; break;
                case '\n': o << "\\n"; break;
                case '\r': o << "\\r"; break;
                case '\t': o << "\\t"; break;
                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        o << "\\u"
                          << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        o << c;
                    }
            }
        }
        return "\"" + o.str() + "\"";
    }

    void print(const SymbolTable::Ptr& symtable) {
        output << "symbol-table:";
        DEBUG_BLOCK_START
        int index = 0;
        for(auto &symbol : symtable->symbols) {
            if(index > 0) {
                newline();
            }
            output << "- " << symbol.second->name <<":";
            DEBUG_BLOCK_START
                output << "flag: " << debugStringOfSymbolFlag(symbol.second->flag);
            DEBUG_BLOCK_END
            index ++;
        }
        DEBUG_BLOCK_END
    }

protected:

    Node::Ptr visit(const ModuleDecl::Ptr& decl) override {
        output << "module:";
        DEBUG_BLOCK_START
        output << "simple-name: " << escapeString(decl->getSimpleName());
        newline();
        print(decl->symtable);
        newline();
        output << "members:";
        DEBUG_BLOCK_START
        for(const auto& member : decl->statements) {
            newline();
            output << "- ";
            NodeVisitor::visit(member);
        }
        DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        
        return decl;
    }

    Node::Ptr visit(const ClassDecl::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const VarDecl::Ptr& decl) override {
        output << "varDecl:";
        DEBUG_BLOCK_START
        output << "simple-name: " << escapeString(decl->getSimpleName());
        newline();
        visit(decl->pattern);
        if(decl->initializer != nullptr) {
            newline();
            NodeVisitor::visit(decl->initializer);
        }
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const TypeIdentifier::Ptr& decl) override {
        output << "kind:";
        incTab();
        newline();
        NodeVisitor::visit(decl->identifier);
        decTab();
        return decl;
    }

    Node::Ptr visit(const FuncCallExpr::Ptr& decl) override {
        output << "funcCallExpr:";
        DEBUG_BLOCK_START
            output << "callee-func-simple-name: " << decl->getCalleeFuncSimpleName();
            newline();
            output << "identifier:";
            DEBUG_BLOCK_START
                NodeVisitor::visit(decl->identifier);
            DEBUG_BLOCK_END
            newline();
            output << "arguments:";
            DEBUG_BLOCK_START
            for(const auto& argument: decl->arguments) {
                output << "- ";
                visit(argument);
                newline();
            }
            DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const MemberFuncCallExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const ArguCallExpr::Ptr& decl) override {
        output << "arguCallExpr:";
        DEBUG_BLOCK_START
            output << "label:";
            DEBUG_BLOCK_START
            visit(decl->label);
            DEBUG_BLOCK_END
            newline();
            output << "expr:";
            DEBUG_BLOCK_START
            NodeVisitor::visit(decl->expr);
            DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const LiteralExpr::Ptr& decl) override {
        output << "literal: " << decl->literal->rawValue;
        return decl;
    }

    Node::Ptr visit(const PrefixExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const IdentifierExpr::Ptr& decl) override {
        output << "identifierExpr:";
        DEBUG_BLOCK_START
        output << "simple-name: " << escapeString(decl->getSimpleName());
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const Expr::Ptr& decl) override {
        output << "expr:";
        DEBUG_BLOCK_START
        if(decl->prefix != nullptr) {
            output << "prefix:";
            DEBUG_BLOCK_START
                NodeVisitor::visit(decl->prefix);
            DEBUG_BLOCK_END
        }

        newline();
        output << "binaries:";
        DEBUG_BLOCK_START
            for(const auto& param: decl->binaries) {
                output << "- ";
                NodeVisitor::visit(param);
                newline();
            }
        DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const AssignExpr::Ptr& decl) override {
        output << "assignExpr:";
        DEBUG_BLOCK_START
        if(decl->expr != nullptr) {
            output << "expr:";
            DEBUG_BLOCK_START
            NodeVisitor::visit(decl->expr);
            DEBUG_BLOCK_END
        }

        newline();
        if(decl->left != nullptr) {
            output << "left:";
            DEBUG_BLOCK_START
            NodeVisitor::visit(decl->left);
            DEBUG_BLOCK_END
        }


        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const BinaryExpr::Ptr& decl) override {
        output << "binaryExpr:";
        DEBUG_BLOCK_START
        output << "op:";
        DEBUG_BLOCK_START
            visit(decl->op);
        DEBUG_BLOCK_END
        newline();
        output << "expr:";
        DEBUG_BLOCK_START
        NodeVisitor::visit(decl->expr);
        DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const OperatorExpr::Ptr& decl) override {
        output << "operatorExpr:";
        DEBUG_BLOCK_START
        output << "token: " << decl->token->rawValue;
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const ParenthesizedExpr::Ptr& decl) override {
        output << "parenthesizedExpr:";
        DEBUG_BLOCK_START
        output << "expr: ";
        DEBUG_BLOCK_START
            NodeVisitor::visit(decl->expr);
        DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const IfStmt::Ptr& decl) override {
        output << "ifStmt:";
        DEBUG_BLOCK_START
        output << "condition:";
        DEBUG_BLOCK_START
            NodeVisitor::visit(decl->condition);
        DEBUG_BLOCK_END
        newline();
        output << "if-block:";
        DEBUG_BLOCK_START
            NodeVisitor::visit(decl->ifCodeBlock);
        DEBUG_BLOCK_END
        newline();
        output << "else-block:";
        DEBUG_BLOCK_START
        NodeVisitor::visit(decl->elseCodeBlock);
        DEBUG_BLOCK_END
        newline();
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const WhileStmt::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const StmtsBlock::Ptr& decl) override {
        output << "statements:";
        incTab();
        for(const auto& statement: decl->statements) {
            newline();
            output << "- ";
            NodeVisitor::visit(statement);
        }
        decTab();
        return decl;
    }

    Node::Ptr visit(const FuncDecl::Ptr& decl) override {
        output << "funcDecl:";
        incTab();
        newline();
        output << "simple-name: " << escapeString(decl->getSimpleName());
        if(decl->parameterClause != nullptr) {
            newline();
            NodeVisitor::visit(decl->parameterClause);
        }

        if(decl->identifier != nullptr) {
            newline();
            NodeVisitor::visit(decl->identifier);
        }

        if(decl->returnType != nullptr) {
            newline();
            output << "return-kind:";
            incTab();
            newline();
            NodeVisitor::visit(decl->returnType);
            decTab();
        }

        newline();
        NodeVisitor::visit(decl->codeBlock);

        decTab();
        return decl;
    }

    Node::Ptr visit(const ParameterClause::Ptr& decl) override {
        output << "paramClause:";
        incTab();
        newline();
        output << "parameters:";
        incTab();

        for(const auto& param: decl->parameters) {
            newline();
            output << "- ";
            visit(param);
        }
        newline();
        decTab();
        return decl;
    }

    Node::Ptr visit(const Pattern::Ptr& decl) override {
        output << "patten:";
        incTab();
        newline();
        visit(decl->identifier);
        if(decl->typeExpr != nullptr) {
            newline();
            NodeVisitor::visit(decl->typeExpr);
        }
        decTab();
        return  decl;
    }

    Node::Ptr visit(const ReturnStmt::Ptr& decl) override {
        output << "return:" ;
        return decl;
    }

    Node::Ptr visit(const SelfExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const ArrayLiteralExpr::Ptr& decl) override {
        output << "arrayLiteralExpr:";
        DEBUG_BLOCK_START
        auto i = 0;
        for(const auto& expr : decl->items) {
            output << "- " << i ++ << ": ";
            DEBUG_BLOCK_START
                NodeVisitor::visit(expr);
            DEBUG_BLOCK_END
            if(i < decl->items.size()) {
                newline();
            }
        }

        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const DictLiteralExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const MemberAccessExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const MemberAssignExpr::Ptr& decl) override {
        output << "memberAssignExpr:";
        DEBUG_BLOCK_START
            output << "member:";
            DEBUG_BLOCK_START
            visit(decl->member);
            DEBUG_BLOCK_END
            newline();
            output << "expr:";
            DEBUG_BLOCK_START
            NodeVisitor::visit(decl->expr);
            DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const SubscriptExpr::Ptr& decl) override {
        output << "subscriptExpr:";
        DEBUG_BLOCK_START
        output << "identifier:";
        DEBUG_BLOCK_START
        NodeVisitor::visit(decl->identifier);
        DEBUG_BLOCK_END
        newline();
        output << "indexExpr:";
        DEBUG_BLOCK_START
        NodeVisitor::visit(decl->indexExpr);
        DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const ArrayType::Ptr& decl) override {
        output << "array:";
        DEBUG_BLOCK_START
        output << "type:";
        DEBUG_BLOCK_START
        NodeVisitor::visit(decl->valueType);
        DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(const ImportStmt::Ptr& decl) override {
        assert(false);
        return decl;
    }
};


struct TypeDefDebugPrinter : YMLPrinter {

    explicit TypeDefDebugPrinter(const std::string& filename):
            YMLPrinter(filename) {}

    void print(const std::vector<Type::Ptr> typedefs) {
        output << "types:";
        DEBUG_BLOCK_START
        auto i = 0;
        for(const auto& tf : typedefs) {
            output << "- type:";
            DEBUG_BLOCK_START
                output << "index: " << i;
                newline();
            switch (tf->kind) {
                case ValueType::Module:
                    output << "kind: Module" ;
                    newline();
                    print(std::static_pointer_cast<ModuleType>(tf));
                    break;
                case ValueType::Nil:
                    output << "kind: Nil" ;
                    break;
                case ValueType::Bool:
                    output << "kind: Boolean" ;
                    break;
                case ValueType::Int:
                    output << "kind: Integer";
                    break;
                case ValueType::Void:
                    output << "kind: Void";
                    break;
                case ValueType::Any:
                    output << "kind: Any";
                    break;
                case ValueType::Class:
                    output << "kind: Class";
                    break;
                case ValueType::Block:
                    output << "kind: Block";
                    newline();
                    print(std::static_pointer_cast<BlockType>(tf));
                    break;
                case ValueType::Func:
                    output << "kind: Func";
                    newline();
                    print(std::static_pointer_cast<FuncType>(tf));
                    break;
                case ValueType::Unspecified:
                    output << "kind: Unspecified";
                    newline();
                    break;
                default:
                    assert(false);
            }

            DEBUG_BLOCK_END
            newline();
            i ++ ;
        }
        DEBUG_BLOCK_END
    }

    void print(const FuncType::Ptr& func) {
        output<< "name: " << func->name;
        if(func->funcKind == VM_Func && func->instructions.size() > 0 ) {
            newline();
            output << "instructions:";
            DEBUG_BLOCK_START
            auto i = 0;
            for(const auto& instruction: func->instructions) {
                if(i > 0){
                    newline();
                }
                output << "- " << i++ << ": " << debugPrint(instruction);
            }
            DEBUG_BLOCK_END
        }
    }

    void print(const ModuleType::Ptr& fileModule) {
        output << "name: " << fileModule->name;
        newline();
        output << "instructions:";
        DEBUG_BLOCK_START
        auto i = 0;
        for(const auto& instruction: fileModule->instructions) {
            if(i > 0){
                newline();
            }
            output << "- " << i++ << ": " << debugPrint(instruction);
        }
        DEBUG_BLOCK_END
    }

    void print(const BlockType::Ptr& block) {
        output << "variables:";
        DEBUG_BLOCK_START
        auto i = 0;
        for(const auto& variable : block->localVars) {
            if (i > 0) {
                newline();
            }
//            output << "- variable: " << variable->typeSlot;
            i ++;
        }
        DEBUG_BLOCK_END
    }
};

#endif
