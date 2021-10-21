#ifndef __joyeer_compiler_node_debugprint_h__
#define __joyeer_compiler_node_debugprint_h__

#include "joyeer/compiler/node+visitor.h"
#include <fstream>
#include <sstream>

// ASM node printer
struct NodeDebugPrinter : public NodeVisitor {

#define DEBUG_BLOCK_START \
    incTab();             \
    newline();

#define DEBUG_BLOCK_END decTab();

    explicit NodeDebugPrinter(const std::string& filename);

    // print AST into debug file
    void print(const Node::Ptr& node) {
        NodeVisitor::visit(node);
    }

    void close();
    
protected:
    void printTab();
    void incTab();
    void decTab();

    void newline() {
        output << std::endl;
        printTab();
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
            output << "- " << symbol.second->name;
            index ++;
        }
        DEBUG_BLOCK_END
    }

protected:

    Node::Ptr visit(FileModuleDecl::Ptr decl) override {
        output << "fileModule:";
        DEBUG_BLOCK_START
        output << "simple-name: " << escapeString(decl->getSimpleName());
        newline();
        print(decl->symtable);
        if(decl->members != nullptr) {
            newline();
            output << "members:";
            DEBUG_BLOCK_START
            visit(decl->members);
            DEBUG_BLOCK_END
        }

        if(!decl->staticFields.empty()) {
            newline();
            output << "static-fields:";
            incTab();
            for(const auto& var: decl->staticFields) {
                newline();
                output << "- ";
                NodeVisitor::visit(var);
            }
            decTab();
        }

        // print instance fields
        if(!decl->instanceFields.empty() ) {
            newline();
            output << "instance-fields:";
            incTab();
            for(const auto& var: decl->instanceFields) {
                newline();
                output << "- ";
                NodeVisitor::visit(var);
            }
            decTab();
        }

        // print static constructor
        if(decl->staticConstructor != nullptr) {
            newline();
            output << "static-constructor:";
            incTab();
            newline();
            visit(decl->staticConstructor);
            decTab();
        }

        DEBUG_BLOCK_END
        
        return decl;
    }

    Node::Ptr visit(ClassDecl::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(VarDecl::Ptr decl) override {
        output << "varDecl:";
        incTab();
        newline();
        output << "simple-name: " << escapeString(decl->getSimpleName());
        newline();
        visit(decl->pattern);
        decTab();
        return decl;
    }

    Node::Ptr visit(Type::Ptr decl) override {
        output << "type:";
        incTab();
        newline();
        NodeVisitor::visit(decl->identifier);
        decTab();
        return decl;
    }

    Node::Ptr visit(FuncCallExpr::Ptr decl) override {
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
            }
            DEBUG_BLOCK_END
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(MemberFuncCallExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(ArguCallExpr::Ptr decl) override {
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

    Node::Ptr visit(LiteralExpr::Ptr decl) override {
        output << "literal: " << decl->literal->rawValue;
        return decl;
    }

    Node::Ptr visit(PrefixExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(IdentifierExpr::Ptr decl) override {
        output << "identifierExpr:";
        DEBUG_BLOCK_START
        output << "simple-name: " << escapeString(decl->getSimpleName());
        DEBUG_BLOCK_END
        return decl;
    }

    Node::Ptr visit(Expr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(AssignExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(BinaryExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(OperatorExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(ParenthesizedExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(IfStmt::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(WhileStmt::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(StmtsBlock::Ptr decl) override {
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

    Node::Ptr visit(FuncDecl::Ptr decl) override {
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
            output << "return-type:";
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

    Node::Ptr visit(ParameterClause::Ptr decl) override {
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

    Node::Ptr visit(Pattern::Ptr decl) override {
        output << "patten:";
        incTab();
        newline();
        visit(decl->identifier);
        if(decl->type != nullptr) {
            newline();
            NodeVisitor::visit(decl->type);
        }
        decTab();
        return  decl;
    }

    Node::Ptr visit(ReturnStmt::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(SelfExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(ArrayLiteralExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(DictLiteralExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(MemberAccessExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(MemberAssignExpr::Ptr decl) override {
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

    Node::Ptr visit(SubscriptExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(ArrayType::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(FileImportStmt::Ptr decl) override {
        return decl;
    }
    
private:
    int size = 0;
    std::ofstream output;
};

#endif
