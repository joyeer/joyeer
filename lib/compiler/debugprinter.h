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

    Node::Ptr visit(const FileModuleDecl::Ptr& decl) override {
        output << "module:";
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
        assert(false);
        return decl;
    }

    Node::Ptr visit(const AssignExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const BinaryExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const OperatorExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const ParenthesizedExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const IfStmt::Ptr& decl) override {
        assert(false);
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
        if(decl->type != nullptr) {
            newline();
            NodeVisitor::visit(decl->typeNode);
        }
        decTab();
        return  decl;
    }

    Node::Ptr visit(const ReturnStmt::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const SelfExpr::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const ArrayLiteralExpr::Ptr& decl) override {
        assert(false);
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
        assert(false);
        return decl;
    }

    Node::Ptr visit(const ArrayType::Ptr& decl) override {
        assert(false);
        return decl;
    }

    Node::Ptr visit(const FileImportStmt::Ptr& decl) override {
        assert(false);
        return decl;
    }
};


struct TypeDefDebugPrinter : YMLPrinter {

    explicit TypeDefDebugPrinter(const std::string& filename):
            YMLPrinter(filename) {}

    void print(const std::vector<Type::Ptr> typedefs) {
        output << "typedefs:";
        DEBUG_BLOCK_START
        auto i = 0;
        for(const auto& tf : typedefs) {
            output << "- typedef:";
            DEBUG_BLOCK_START
                output << "index: " << i;
                newline();
            switch (tf->kind) {
                case TypeKind::FileModule:
                    output << "kind: FileModule" ;
                    newline();
                    print(std::static_pointer_cast<ModuleType>(tf));
                    break;
                case TypeKind::Nil:
                    output << "kind: Nil" ;
                    break;
                case TypeKind::Boolean:
                    output << "kind: Boolean" ;
                    break;
                case TypeKind::Integer:
                    output << "kind: Integer";
                    break;
                case TypeKind::Void:
                    output << "kind: Void";
                    break;
                case TypeKind::Long:
                    output << "kind: Long";
                    break;
                case TypeKind::Any:
                    output << "kind: Any";
                    break;
                case TypeKind::Class:
                    assert(false);
                case TypeKind::Block:
                    output << "kind: Block";
                    newline();
                    print(std::static_pointer_cast<BlockType>(tf));
                    break;
                case TypeKind::Function:
                    output << "kind: Function";
                    newline();
                    print(std::static_pointer_cast<FuncType>(tf));
                    break;
                case TypeKind::Variable:
                    output << "kind: Variable";
                    newline();
                    print(std::static_pointer_cast<VariableType>(tf));
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
            output << "- variable: " << variable->address;
            i ++;
        }
        DEBUG_BLOCK_END
    }

    void print(const VariableType::Ptr& variable) {
        output << "name: " << variable->name;
        newline();
        output << "access-flag: " << debugAccessFlag(variable->accessFlags);
    }
};

#endif
