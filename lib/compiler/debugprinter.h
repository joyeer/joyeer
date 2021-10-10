#ifndef __joyeer_compiler_node_debugprint_h__
#define __joyeer_compiler_node_debugprint_h__

#include "joyeer/compiler/node+visitor.h"
#include <fstream>
#include <sstream>

// ASM node printer
struct NodeDebugPrinter : public NodeVisitor {
    explicit NodeDebugPrinter(const std::string& filename);
    
    void print(const Node::Ptr& node) {
        NodeVisitor::visit(node);
    }
    void close();
    
protected:
    void print(const std::vector<Node::Ptr>& nodes);
    void print(const SymbolTable::Ptr& symtable);
    void print(const Symbol::Ptr& symbol);
    void printTab();
    void incTab();
    void decTab();

    void newlineWithTab(int incTab) {
        size += incTab;
        output << std::endl;
        printTab();
    }

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


protected:

    Node::Ptr visit(FileModuleDecl::Ptr decl) override {
        output << "fileModule:";
        newlineWithTab(1);
        output << "simple-name: " << escapeString(decl->getSimpleName());
        newlineWithTab(0);
        output << "static-fields:";
        incTab();
        for(const auto& var: decl->staticFields) {
            newline();
            output << "- ";
            NodeVisitor::visit(var);
        }
        decTab();

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
        return decl;
    }

    Node::Ptr visit(MemberFuncCallExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(ArguCallExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(LiteralExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(PrefixExpr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(IdentifierExpr::Ptr decl) override {
        output << "identifierExpr:";
        incTab();
        newline();
        output << "simple-name: " << escapeString(decl->getSimpleName());
        decTab();
        return decl;
    }

    Node::Ptr visit(Expr::Ptr decl) override {
        return decl;
    }

    Node::Ptr visit(AssignmentExpr::Ptr decl) override {
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
        return decl;
    }

    Node::Ptr visit(FuncDecl::Ptr decl) override {
        output << "funcDecl:";
        incTab();
        newline();
        output << "simple-name: " << escapeString(decl->getSimpleName());
        decTab();
        return decl;
    }

    Node::Ptr visit(ParameterClause::Ptr decl) override {
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
