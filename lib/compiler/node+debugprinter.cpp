#include "joyeer/compiler/node+debugprinter.h"
#include "joyeer/compiler/node+types.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NodeDebugPrinter::NodeDebugPrinter(const std::string filename) {
    output.open(filename);
}

void NodeDebugPrinter::print(SymbolTable::Ptr symtable) {
    if(symtable == nullptr) {
        return;
    }
    output << std::endl;
    printTab();
    output << "#symtable {" << std::endl;
    incTab();
    for(auto symbol: symtable->symbols) {
        printTab();
        output << symbol.second->name << ": " << symbol.second->addressOfFunc <<  ", " << symbol.second->addressOfVariable << std::endl;
    }
    decTab();
    printTab();
    output << "}";
}

void NodeDebugPrinter::print(Symbol::Ptr symbol) {
    if(symbol == nullptr) {
        return;
    }
    output << std::endl;
    printTab();
    output << "@symbol-> name:\""<< symbol->name << "\", flag:\"" << debugStringOfSymbolFlag(symbol->flag) << "\", mutable:" << symbol->isMutable;
}

void NodeDebugPrinter::print(std::vector<Node::Ptr> nodes) {
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

void NodeDebugPrinter::print(Node::Ptr node) {
    if(node == nullptr) {
        return;
    }
    if (size > 0 ) {
        output << std::endl;
    }
    printTab();
    
    switch (node->kind) {
        case sourceBlock: {
            
            auto n = std::static_pointer_cast<FileModuleNode>(node);
            output << "+sourceBLock(filename: \"" << n->filename << "\")" ;
            incTab();
            print(n->symtable);
            print(n->symbol);
            for(auto statement: n->statements) {
                print(statement);
            }
            decTab();
        }
            break;
        case type: {
            output << "+type" ;
            auto n = std::static_pointer_cast<Type>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->identifier);
            decTab();
        }
            break;
        case arrayType:
            output << "+arrayType" ;
            break;
        case dictType:
            output << "+dictType" ;
            break;
        case pattern: {
            output << "+pattern" ;
            auto n = std::static_pointer_cast<Pattern>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->identifier);
            if(n->type != nullptr) {
                print(n->type);
            }
            decTab();
        }
            break;
        case importDecl:
            output << "+importDecl" ;
            break;
        case letDecl: {
            output << "+constantDecl" ;
            auto n = std::static_pointer_cast<LetDecl>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->pattern);
            print(n->initializer);
            decTab();
        }
            
            break;
        case varDecl: {
            output << "+varDecl" ;
            auto n = std::static_pointer_cast<VarDecl>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->pattern);
            print(n->initializer);
            decTab();
        }
            break;
        case funcDecl: {
            output << "+funcDecl" ;
            auto n = std::static_pointer_cast<FuncDecl>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->identifier);
            print(n->parameterClause);
            print(n->returnType);
            print(n->codeBlock);
            decTab();
        }
            break;
        case constructorDecl: {
            output << "+constructorDecl" ;
            auto n = std::static_pointer_cast<ConstructorDecl>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->parameterClause);
            print(n->codeBlock);
            decTab();
        }
            break;
        case classDecl: {
            output << "+classDecl" ;
            auto members = std::static_pointer_cast<ClassDecl>(node);
            incTab();
            print(members->symtable);
            print(members->symbol);
            print(members->members);
            decTab();
        }
            break;
        case parameterClause: {
            output << "+parameterClause" ;
            auto n = std::static_pointer_cast<ParameterClause>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            std::vector<Node::Ptr> parameters;
            for(auto p: n->parameters) {
                parameters.push_back(p);
            }
            print(parameters);
            decTab();
        }
            break;
        case codeBlock: {
            auto n = std::static_pointer_cast<CodeBlock>(node);
            output << "+codeBlock" ;
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->statements);
            decTab();
        }
            break;
        case forInStatement:
            output << "+forInStatement" ;
            break;
        case ifStatement: {
            auto n = std::static_pointer_cast<IfStatement>(node);
            output << "+ifStatement" ;
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->condition);
            print(n->ifCodeBlock);
            print(n->elseCodeBlock);
            decTab();
        }
            break;
        case expr: {
            output << "+expr" ;
            auto n = std::static_pointer_cast<Expr>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            if(n->prefix != nullptr) {
                print(n->prefix);
            }
            if(n->binaries.size() > 0) {
                print(n->binaries);
            }
            if(n->nodes.size() > 0) {
                print(n->nodes);
            }
            decTab();
        }
            break;
        case selfExpr:
            output << "+selfExpr" ;
            break;
        case postfixExpr: {
            output << "+postfixExpr" ;
            auto n = std::static_pointer_cast<PostfixExpr>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->expr);
            print(n->op);
            decTab();
        }
            break;
        case prefixExpr: {
            output << "+prefixExpr" ;
            auto n = std::static_pointer_cast<PrefixExpr>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            if(n->op != nullptr) {
                print(n->op);
            }
            if(n->expr != nullptr) {
                print(n->expr);
            }
            decTab();
        }
            break;
        case identifierExpr: {
            auto n = std::static_pointer_cast<IdentifierExpr>(node);
            output << "+identifierExpr(" << n->token->rawValue << ")"; ;
            incTab();
            print(n->symtable);
            print(n->symbol);
            decTab();
        }
            break;
        case parenthesizedExpr: {
            auto n = std::static_pointer_cast<ParenthesizedExpr>(node);
            output << "+parenthesizedExpr" ;
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->expr);
            decTab();
        }
            break;
        case arguCallExpr: {
            output << "+arguCallExpr" ;
            auto n = std::static_pointer_cast<ArguCallExpr>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->label);
            print(n->expr);
            decTab();
        }
            break;
        case funcCallExpr: {
            output << "+functionCallExpr" ;
            auto n = std::static_pointer_cast<FuncCallExpr>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->identifier);
            incTab();
            for(auto argu: n->arguments) {
                print(argu);
            }
            decTab();
            decTab();
        }
            break;
        case memberAccessExpr: {
            auto n = std::static_pointer_cast<MemberAccessExpr>(node);
            output << "+memberAccessExpr" ;
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->parent);
            print(n->member);
            decTab();
        }
            break;
        case memberFuncCallExpr: {
            auto n = std::static_pointer_cast<MemberFuncCallExpr>(node);
            output << "+memberFuncCallExpr";
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->parent);
            print(n->member);
            incTab();
            for(auto argu: n->arguments) {
                print(argu);
            }
            decTab();
            decTab();
        }
            break;
        case literalExpr: {
            auto n = std::static_pointer_cast<LiteralExpr>(node);
            output << "+literalExpr(" << n->literal->rawValue << ")" ;
            print(n->symtable);
            print(n->symbol);
        }
            break;
        case arrayLiteralExpr: {
            output << "+arrayLiteralExpr" ;
            auto n = std::static_pointer_cast<ArrayLiteralExpr>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->items);
            decTab();
        }
            break;
        case dictLiteralExpr:
            output << "+dictLiteralExpr" ;
            break;
        case assignmentExpr: {
            output << "+assignmentExpr" ;
            auto n = std::static_pointer_cast<AssignmentExpr>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->left);
            print(n->expr);
            decTab();
        }
            break;
        case binaryExpr: {
            output << "+binaryExpr" ;
            auto n = std::static_pointer_cast<BinaryExpr>(node);
            incTab();
            print(n->symtable);
            print(n->symbol);
            if(n->op != nullptr) {
                print(n->op);
            }
            if(n->expr != nullptr) {
                print(n->expr);
            }
            decTab();
        }
            break;
        case operatorExpr: {
            auto n = std::static_pointer_cast<OperatorExpr>(node);
            output << "+operatorExpr(" << n->token->rawValue << ")" ;
            incTab();
            print(n->symtable);
            print(n->symbol);
            decTab();
        }
            break;
        case returnStatement: {
            auto n = std::static_pointer_cast<ReturnStatement>(node);
            output << "+returnStatement";
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->expr);
            decTab();
        }
            break;
        case subscriptExpr: {
            auto n = std::static_pointer_cast<SubscriptExpr>(node);
            output << "+subscriptExpr";
            incTab();
            print(n->symtable);
            print(n->symbol);
            print(n->identifier);
            print(n->indexExpr);
            decTab();
        }
            break;
        case whileStatement: {
            auto n = std::static_pointer_cast<WhileStatement>(node);
            output << "+whileStatement";
            incTab();
            print(n->expr);
            print(n->codeBlock);
            decTab();
        }
            break;
        case fileimportDecl: {
            auto n = std::static_pointer_cast<FileImportDecl>(node);
            output << "+fileimport(\"" << n->stringLiteral->rawValue << "\")";
        }
            break;
        case moduleMemberAccessExpr: {
            auto n = std::static_pointer_cast<ModuleMemberAccessExpr>(node);
            output << "+moduleMemberAccessExpr(\"" << n->moduleClass->name << "\")";
            incTab();
            print(n->member);
            decTab();
        }
            break;
        case moduleFuncCallExpr: {
            auto n = std::static_pointer_cast<ModuleFuncCallExpr>(node);
            output << "+moduleFuncCallExpr";
            incTab();
            print(n->symtable);
            print(n->symbol);
//            print(n->parent);
            print(n->member);
            incTab();
//            for(auto argu: n->arguments) {
//                print(argu);
//            }
            decTab();
            decTab();

        }
            break;
        default:
            assert(false);
    }
}

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
