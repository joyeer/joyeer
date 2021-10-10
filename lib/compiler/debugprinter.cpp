#include "debugprinter.h"
#include "joyeer/compiler/node+stmt.h"
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NodeDebugPrinter::NodeDebugPrinter(const std::string& filename) {
    output.open(filename);
}

void NodeDebugPrinter::print(const SymbolTable::Ptr& symtable) {
    if(symtable == nullptr) {
        return;
    }
    printTab();
    output << "symtable:" << std::endl;
    incTab();
    int index = 0;
    for(const auto& symbol: symtable->symbols) {
        if(index != 0) {
            output << std::endl;
        }
        printTab();
        output << symbol.second->name << ": " << symbol.second->addressOfFunc <<  ", " << symbol.second->addressOfVariable ;
        index = index + 1;
    }
    decTab();
    printTab();
}

void NodeDebugPrinter::print(const Symbol::Ptr& symbol) {
    if(symbol == nullptr) {
        return;
    }
    output << std::endl;
    printTab();
    output << "symbol:" << std::endl;
    incTab();
    printTab();
    output << "name: \""<< symbol->name << "\"" <<std::endl;
    printTab();
    output << "flag: \"" << debugStringOfSymbolFlag(symbol->flag) << "\"";
    decTab();
}

void NodeDebugPrinter::print(const std::vector<Node::Ptr>& nodes) {
    output << std::endl;
    printTab();
    
    output << "[";
    incTab();
    
    for(auto n : nodes) {
        print(n);
    }
    decTab();
    output << std::endl;
    printTab();
    
    output << "]";
    
}
//
//void NodeDebugPrinter::print(const Node::Ptr& node) {
//    if(node == nullptr) {
//        return;
//    }
//    if (size > 0 ) {
//        output << std::endl;
//    }
//    printTab();
//
//    switch (node->kind) {
//        case SyntaxKind::fileModule:
//        case SyntaxKind::classDecl: {
//            auto n = std::static_pointer_cast<FileModuleDecl>(node);
//            if(node->kind == SyntaxKind::fileModule) {
//                output << "fileModule:" << std::endl;
//            } else {
//                output << "classDecl:" << std::endl;
//            }
//            incTab();
//            printTab();
//            output << "simple-name: " << n->getSimpleName() << std::endl;
//            print(n->symtable);
//            print(n->symbol);
//
//            // origin members
//            print(n->members);
//
//            output << "static-variables:" << std::endl;
//            incTab();
//            // print static fields
//            for(const auto& var: n->staticFields) {
//                print(var);
//            }
//            decTab();
//            // print fields
//            for(const auto& var: n->instanceFields) {
//                print(var);
//            }
//            // print static constructor
//            print(n->staticConstructor);
//            // print constructors
//            for (const auto& func: n->constructors) {
//                print(func);
//            }
//            // print static functions
//            for(const auto& func: n->staticMethods) {
//                print(func);
//            }
//            // print instance functions
//            for(const auto& func: n->instanceMethods) {
//                print(func);
//            }
//            decTab();
//        }
//            break;
//        case SyntaxKind::type: {
//            output << "type:" ;
//            auto n = std::static_pointer_cast<Type>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->identifier);
//            decTab();
//        }
//            break;
//        case SyntaxKind::arrayType:
//            output << "arrayType:" ;
//            break;
//        case SyntaxKind::dictType:
//            output << "dictType:" ;
//            break;
//        case SyntaxKind::pattern: {
//            output << "pattern:" ;
//            auto n = std::static_pointer_cast<Pattern>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->identifier);
//            if(n->type != nullptr) {
//                print(n->type);
//            }
//            decTab();
//        }
//            break;
//        case SyntaxKind::importStmt:
//            output << "+importStatement" ;
//            break;
//        case SyntaxKind::varDecl: {
//            auto n = std::static_pointer_cast<VarDecl>(node);
//            switch (n->accessFlag) {
//                case NodeAccessFlag::_static:
//                    output << "+static-variable(simple-name:\"" << n->getSimpleName() << "\")" ;
//                    break;
//                default:
//                    output << "+variable(simple-name:\"" << n->getSimpleName() << "\")" ;
//                    break;
//            }
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->pattern);
//            print(n->initializer);
//            decTab();
//        }
//            break;
//        case SyntaxKind::funcDecl: {
//            auto n = std::static_pointer_cast<FuncDecl>(node);
//            if(n->type == FuncType::function) {
//                output << "+function(simple-name:\""<< n->getSimpleName() <<"\")" ;
//            } else if(n->type == FuncType::staticInitializer) {
//                output << "+static-constructor(simple-name:\""<< n->getSimpleName() <<"\")" ;
//            } else if(n->type == FuncType::constructor) {
//                output << "+constructor()";
//            }
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->identifier);
//            print(n->parameterClause);
//            print(n->returnType);
//            print(n->codeBlock);
//            decTab();
//        }
//            break;
//                case SyntaxKind::parameterClause: {
//            output << "+parameterClause" ;
//            auto n = std::static_pointer_cast<ParameterClause>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            std::vector<Node::Ptr> parameters;
//            for(auto p: n->parameters) {
//                parameters.push_back(p);
//            }
//            print(parameters);
//            decTab();
//        }
//            break;
//        case SyntaxKind::stmtsBlock: {
//            auto n = std::static_pointer_cast<StmtsBlock>(node);
//            output << "+codeBlock" ;
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->statements);
//            decTab();
//        }
//            break;
//        case SyntaxKind::forInStmt:
//            output << "+forInStatement" ;
//            break;
//        case SyntaxKind::ifStmt: {
//            auto n = std::static_pointer_cast<IfStmt>(node);
//            output << "+ifStatement" ;
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->condition);
//            print(n->ifCodeBlock);
//            print(n->elseCodeBlock);
//            decTab();
//        }
//            break;
//        case SyntaxKind::expr: {
//            output << "+expr" ;
//            auto n = std::static_pointer_cast<Expr>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            if(n->prefix != nullptr) {
//                print(n->prefix);
//            }
//            if(n->binaries.size() > 0) {
//                print(n->binaries);
//            }
//            if(n->nodes.size() > 0) {
//                print(n->nodes);
//            }
//            decTab();
//        }
//            break;
//        case SyntaxKind::selfExpr:
//            output << "+selfExpr" ;
//            break;
//        case SyntaxKind::postfixExpr: {
//            output << "+postfixExpr" ;
//            auto n = std::static_pointer_cast<PostfixExpr>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->expr);
//            print(n->op);
//            decTab();
//        }
//            break;
//        case SyntaxKind::prefixExpr: {
//            output << "+prefixExpr" ;
//            auto n = std::static_pointer_cast<PrefixExpr>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            if(n->op != nullptr) {
//                print(n->op);
//            }
//            if(n->expr != nullptr) {
//                print(n->expr);
//            }
//            decTab();
//        }
//            break;
//        case SyntaxKind::identifierExpr: {
//            auto n = std::static_pointer_cast<IdentifierExpr>(node);
//            output << "+identifierExpr(" << n->token->rawValue << ")"; ;
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            decTab();
//        }
//            break;
//        case SyntaxKind::parenthesizedExpr: {
//            auto n = std::static_pointer_cast<ParenthesizedExpr>(node);
//            output << "+parenthesizedExpr" ;
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->expr);
//            decTab();
//        }
//            break;
//        case SyntaxKind::arguCallExpr: {
//            output << "+arguCallExpr" ;
//            auto n = std::static_pointer_cast<ArguCallExpr>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->label);
//            print(n->expr);
//            decTab();
//        }
//            break;
//        case SyntaxKind::funcCallExpr: {
//            output << "+functionCallExpr" ;
//            auto n = std::static_pointer_cast<FuncCallExpr>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->identifier);
//            incTab();
//            for(auto argu: n->arguments) {
//                print(argu);
//            }
//            decTab();
//            decTab();
//        }
//            break;
//        case SyntaxKind::memberAssignExpr: {
//            auto n = std::static_pointer_cast<MemberAssignExpr>(node);
//            output << "+member-assign-expr" ;
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->member);
//            print(n->expr);
//            decTab();
//        }
//            break;
//        case SyntaxKind::memberAccessExpr: {
//            auto n = std::static_pointer_cast<MemberAccessExpr>(node);
//            output << "+memberAccessExpr" ;
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->callee);
//            print(n->member);
//            decTab();
//        }
//            break;
//        case SyntaxKind::memberFuncCallExpr: {
//            auto n = std::static_pointer_cast<MemberFuncCallExpr>(node);
//            output << "+memberFuncCallExpr";
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->callee);
//            print(n->member);
//            incTab();
//            for(auto argu: n->arguments) {
//                print(argu);
//            }
//            decTab();
//            decTab();
//        }
//            break;
//        case SyntaxKind::literalExpr: {
//            auto n = std::static_pointer_cast<LiteralExpr>(node);
//            output << "+literalExpr(" << n->literal->rawValue << ")" ;
//            print(n->symtable);
//            print(n->symbol);
//        }
//            break;
//        case SyntaxKind::arrayLiteralExpr: {
//            output << "+arrayLiteralExpr" ;
//            auto n = std::static_pointer_cast<ArrayLiteralExpr>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->items);
//            decTab();
//        }
//            break;
//        case SyntaxKind::dictLiteralExpr:
//            output << "+dictLiteralExpr" ;
//            break;
//        case SyntaxKind::assignmentExpr: {
//            output << "+assignmentExpr" ;
//            auto n = std::static_pointer_cast<AssignmentExpr>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->left);
//            print(n->expr);
//            decTab();
//        }
//            break;
//        case SyntaxKind::binaryExpr: {
//            output << "+binaryExpr" ;
//            auto n = std::static_pointer_cast<BinaryExpr>(node);
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            if(n->op != nullptr) {
//                print(n->op);
//            }
//            if(n->expr != nullptr) {
//                print(n->expr);
//            }
//            decTab();
//        }
//            break;
//        case SyntaxKind::operatorExpr: {
//            auto n = std::static_pointer_cast<OperatorExpr>(node);
//            output << "+operatorExpr(" << n->token->rawValue << ")" ;
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            decTab();
//        }
//            break;
//        case SyntaxKind::returnStmt: {
//            auto n = std::static_pointer_cast<ReturnStmt>(node);
//            output << "+returnStatement";
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->expr);
//            decTab();
//        }
//            break;
//        case SyntaxKind::subscriptExpr: {
//            auto n = std::static_pointer_cast<SubscriptExpr>(node);
//            output << "+subscriptExpr";
//            incTab();
//            print(n->symtable);
//            print(n->symbol);
//            print(n->identifier);
//            print(n->indexExpr);
//            decTab();
//        }
//            break;
//        case SyntaxKind::whileStmt: {
//            auto n = std::static_pointer_cast<WhileStmt>(node);
//            output << "+whileStatement";
//            incTab();
//            print(n->expr);
//            print(n->codeBlock);
//            decTab();
//        }
//            break;
//        case SyntaxKind::fileimportStmt: {
//            auto n = std::static_pointer_cast<FileImportStmt>(node);
//            output << "+fileimport(\"" << n->stringLiteral->rawValue << "\")";
//        }
//            break;
//        default:
//            assert(false);
//    }
//}

void NodeDebugPrinter::printTab() {
    output << std::string(size*2, ' ') ;
}

void NodeDebugPrinter::incTab() {
    size ++;
}

void NodeDebugPrinter::decTab() {
    size --;
}

void NodeDebugPrinter::close() {
    output.flush();
    output.close();
}
