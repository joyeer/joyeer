#include "joyeer/compiler/node.h"
#include "joyeer/runtime/buildin.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <cassert>


Node::Node(SyntaxKind k): kind(k) {
}

std::string Node::getName() {
    return "";
}

std::string Node::getTypeName() {
    return "";
}

OperatorExpr::OperatorExpr(Token::Ptr token):
Node(SyntaxKind::operatorExpr),
token(token) {
    
}

LetDecl::LetDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer):
Node(SyntaxKind::letDecl),
pattern(pattern),
initializer(initializer) {
}

VarDecl::VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer):
Node(SyntaxKind::varDecl),
pattern(pattern),
initializer(initializer) {
}

ClassDecl::ClassDecl(Token::Ptr name, std::vector<Node::Ptr> members):
Node(SyntaxKind::classDecl),
name(name),
members(members) {
}

std::string ClassDecl::getName() {
    return name->rawValue;
}

ParameterClause::ParameterClause(std::vector<std::shared_ptr<Pattern>> parameters):
Node(SyntaxKind::parameterClause),
parameters(parameters) {
}

FuncDecl::FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock):
Node(SyntaxKind::funcDecl),
identifier(identifier),
parameterClause(parameterClause),
returnType(returnType),
codeBlock(codeBlock) {
}

std::string FuncDecl::getTypeName() {
    std::stringstream ss;
    if(identifier != nullptr && identifier->symbol != nullptr) {
        ss << identifier->getTypeName();
        ss << "@";
    }
    ss << std::static_pointer_cast<IdentifierExpr>(identifier)->token->rawValue << "(";
    for(auto p: std::static_pointer_cast<ParameterClause>(parameterClause)->parameters) {
        ss << p->identifier->token->rawValue << ":";
    }
    ss << ")";
    return ss.str();
}


Pattern::Pattern(IdentifierExpr::Ptr identifier, Node::Ptr type):
Node(SyntaxKind::pattern),
identifier(identifier),
type(type) {
}

const std::string& Pattern::getIdentifierName() {
    return identifier->token->rawValue;
}


ConstructorDecl::ConstructorDecl(std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock):
Node(SyntaxKind::constructorDecl),
parameterClause(parameterClause),
codeBlock(codeBlock) {
}

const std::string ConstructorDecl::getName(JrType* type) {
    std::stringstream ss;
    ss << type->name << "(";
    auto parameterClause = std::static_pointer_cast<ParameterClause>(this->parameterClause);
    for(auto parameter: parameterClause->parameters) {
        ss << parameter->getIdentifierName() << ":";
    }
    ss << ")";
    return ss.str();
}

Expr::Expr(Node::Ptr prefix, std::vector<Node::Ptr> binaries):
Node(SyntaxKind::expr),
prefix(prefix),
binaries(binaries) {
}

Expr::Expr(std::vector<Node::Ptr> nodes):
Node(SyntaxKind::expr),
prefix(nullptr),
binaries(),
nodes(nodes){
    
}

PostfixExpr::PostfixExpr(Node::Ptr expr, OperatorExpr::Ptr op):
Node(SyntaxKind::postfixExpr),
expr(expr),
op(op) {
}

PrefixExpr::PrefixExpr(OperatorExpr::Ptr op, Node::Ptr expr):
Node(SyntaxKind::prefixExpr),
op(op),
expr(expr) {
    
}

BinaryExpr::BinaryExpr(OperatorExpr::Ptr op, Node::Ptr expr):
Node(SyntaxKind::binaryExpr),
op(op),
expr(expr) {
}

AssignmentExpr::AssignmentExpr(Node::Ptr expr):
Node(SyntaxKind::assignmentExpr),
expr(expr) {
}

ArguCallExpr::ArguCallExpr(IdentifierExpr::Ptr identifier, Node::Ptr expr):
Node(SyntaxKind::arguCallExpr),
label(identifier),
expr(expr) {
}

FuncCallExpr::FuncCallExpr(Node::Ptr expr, std::vector<ArguCallExpr::Ptr> arguments):
Node(SyntaxKind::funcCallExpr),
identifier(expr),
arguments(arguments) {
}

std::string FuncCallExpr::getTypeName() {
    std::stringstream ss;

    ss << identifier->getTypeName() << "(";
    for(auto& argument: arguments) {
        ss << argument->label->token->rawValue;
        ss << ":";
    }
    ss << ")";
    return ss.str();
}

MemberFuncCallExpr::MemberFuncCallExpr(Node::Ptr parent, Node::Ptr member, std::vector<ArguCallExpr::Ptr> arguments):
Node(SyntaxKind::memberFuncCallExpr),
parent(parent),
member(member),
arguments(arguments) {
}

std::string MemberFuncCallExpr::getTypeName() {
    std::stringstream ss;
    
    ss << parent->getTypeName() << "@" << member->getName() << "(";
    for(auto& argument: arguments) {
        ss << argument->label->token->rawValue << ":";
    }
    ss << ")";
    return ss.str();
}

MemberAccessExpr::MemberAccessExpr(std::shared_ptr<Node> parent, std::shared_ptr<Node> member):
Node(SyntaxKind::memberAccessExpr),
parent(parent),
member(member) {
}

std::string MemberAccessExpr::getTypeName() {
    std::stringstream ss;
    ss << parent->getTypeName() << "@" << member->getName();
    return ss.str();
}

LiteralExpr::LiteralExpr(std::shared_ptr<Token> literal):
Node(SyntaxKind::literalExpr),
literal(literal) {
    
}

ArrayLiteralExpr::ArrayLiteralExpr(std::vector<Node::Ptr> items):
Node(arrayLiteralExpr),
items(items) {
}

DictLiteralExpr::DictLiteralExpr(std::vector<std::tuple<Node::Ptr, Node::Ptr>> items):
Node(dictLiteralExpr),
items(items) {
}

IdentifierExpr::IdentifierExpr(std::shared_ptr<Token> token):
Node(SyntaxKind::identifierExpr),
token(token) {
}

std::string IdentifierExpr::getName() {
    return token->rawValue;
}

std::string IdentifierExpr::getTypeName() {
    if(symbol != nullptr && (symbol->flag == varSymbol || symbol->flag == fieldSymbol)) {
        auto type = Global::types[symbol->addressOfType];
        assert(type != nullptr);
        return type->name;
    } else {
        return token->rawValue;
    }
    
    assert(false);
}

ParenthesizedExpr::ParenthesizedExpr(std::shared_ptr<Node> expr):
Node(SyntaxKind::parenthesizedExpr),
expr(expr) {
}

SelfExpr::SelfExpr(IdentifierExpr::Ptr identifier):
Node(SyntaxKind::selfExpr),
identifier(identifier) {
}

SubscriptExpr::SubscriptExpr(Node::Ptr identifier, Node::Ptr indexExpr):
Node(subscriptExpr),
identifier(identifier),
indexExpr(indexExpr) {
}

Type::Type(Node::Ptr identifier):
Node(SyntaxKind::type),
identifier(identifier) {
}

ArrayType::ArrayType(Node::Ptr type):
Node(SyntaxKind::arrayType),
type(type) {
}

std::string ArrayType::getTypeName() {
    return "Array@Array";
}

DictType::DictType(Node::Ptr keyType, Node::Ptr valueType):
Node(SyntaxKind::dictType),
keyType(keyType),
valueType(valueType) {
}

std::string DictType::getTypeName() {
    return "Map@Map";
}

SourceBlock::SourceBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::sourceBlock),
statements(statements) {

}

std::string SourceBlock::getName() {
    std::filesystem::path p = filename;
    return p.replace_extension().string();
}

std::vector<FileImportDecl::Ptr> SourceBlock::getFileImports() {
    std::vector<FileImportDecl::Ptr> result;
    for(std::vector<Node::Ptr>::const_iterator iterator = statements.begin(); iterator != statements.end(); iterator ++ ) {
        auto node = *iterator;
        if(node->kind == fileimportDecl) {
            result.push_back(std::static_pointer_cast<FileImportDecl>(node));
        }
    }
    return result;
}

FileImportDecl::FileImportDecl(Token::Ptr stringLiteral):
Node(SyntaxKind::fileimportDecl),
stringLiteral(stringLiteral) {
}

const std::string FileImportDecl::getImportedFilename() {
    return stringLiteral->rawValue;
}

ModuleMemberAccessExpr::ModuleMemberAccessExpr(JrModuleClass* moduleClass, Node::Ptr member):
Node(SyntaxKind::moduleMemberAccessExpr),
moduleClass(moduleClass),
member(member) {
}

ModuleFuncCallExpr::ModuleFuncCallExpr(JrModuleClass* moduleClass, Node::Ptr member):
Node(SyntaxKind::moduleFuncCallExpr),
moduleClass(moduleClass),
member(member) {
}

CodeBlock::CodeBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::codeBlock),
statements(statements) {
}

ForInStatement::ForInStatement(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock):
Node(SyntaxKind::forInStatement),
pattern(pattern),
inExpr(inExpr),
codeBlock(codeBlock) {
}

WhileStatement::WhileStatement(Node::Ptr expr, Node::Ptr codeBlock):
Node(SyntaxKind::whileStatement),
expr(expr),
codeBlock(codeBlock) {
}

IfStatement::IfStatement(Node::Ptr condition, Node::Ptr ifCodeBlock, std::shared_ptr<Node> elseCodeBlock):
Node(SyntaxKind::ifStatement),
condition(condition),
ifCodeBlock(ifCodeBlock),
elseCodeBlock(elseCodeBlock) {
}

ReturnStatement::ReturnStatement(Node::Ptr expr):
Node(SyntaxKind::returnStatement),
expr(expr) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Node::Ptr NodeVisitor::visit(Node::Ptr node) {
    switch (node->kind) {
        case sourceBlock:
            return visit(std::static_pointer_cast<SourceBlock>(node));
        case type:
            return visit(std::static_pointer_cast<Type>(node));
        case pattern:
            return visit(std::static_pointer_cast<Pattern>(node));
        case letDecl:
            return visit(std::static_pointer_cast<LetDecl>(node));
        case varDecl:
            return visit(std::static_pointer_cast<VarDecl>(node));
        case funcDecl:
            return visit(std::static_pointer_cast<FuncDecl>(node));
        case constructorDecl:
            return visit(std::static_pointer_cast<ConstructorDecl>(node));
        case classDecl:
            return visit(std::static_pointer_cast<ClassDecl>(node));
        case parameterClause:
            return visit(std::static_pointer_cast<ParameterClause>(node));
        case codeBlock:
            return visit(std::static_pointer_cast<CodeBlock>(node));
        case ifStatement:
            return visit(std::static_pointer_cast<IfStatement>(node));
        case whileStatement:
            return visit(std::static_pointer_cast<WhileStatement>(node));
        case expr:
            return visit(std::static_pointer_cast<Expr>(node));
        case selfExpr:
            return visit(std::static_pointer_cast<SelfExpr>(node));
        case prefixExpr:
            return visit(std::static_pointer_cast<PrefixExpr>(node));
        case identifierExpr:
            return visit(std::static_pointer_cast<IdentifierExpr>(node));
        case parenthesizedExpr:
            return visit(std::static_pointer_cast<ParenthesizedExpr>(node));
        case arguCallExpr:
            return visit(std::static_pointer_cast<ArguCallExpr>(node));
        case funcCallExpr:
            return visit(std::static_pointer_cast<FuncCallExpr>(node));
        case memberAccessExpr:
            return visit(std::static_pointer_cast<MemberAccessExpr>(node));
        case memberFuncCallExpr:
            return visit(std::static_pointer_cast<MemberFuncCallExpr>(node));
        case literalExpr:
            return visit(std::static_pointer_cast<LiteralExpr>(node));
        case arrayLiteralExpr:
            return visit(std::static_pointer_cast<ArrayLiteralExpr>(node));
        case dictLiteralExpr:
            return visit(std::static_pointer_cast<DictLiteralExpr>(node));
        case assignmentExpr:
            return visit(std::static_pointer_cast<AssignmentExpr>(node));
        case binaryExpr:
            return visit(std::static_pointer_cast<BinaryExpr>(node));
        case operatorExpr:
            return visit(std::static_pointer_cast<OperatorExpr>(node));
        case returnStatement:
            return visit(std::static_pointer_cast<ReturnStatement>(node));
        case subscriptExpr:
            return visit(std::static_pointer_cast<SubscriptExpr>(node));
        case arrayType:
            return visit(std::static_pointer_cast<ArrayType>(node));
        case fileimportDecl:
            return visit(std::static_pointer_cast<FileImportDecl>(node));
        case moduleMemberAccessExpr:
            return visit(std::static_pointer_cast<ModuleMemberAccessExpr>(node));
        case moduleFuncCallExpr:
            return visit(std::static_pointer_cast<ModuleFuncCallExpr>(node));
        default:
            assert(false);
    }
}

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
            
            auto n = std::static_pointer_cast<SourceBlock>(node);
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
