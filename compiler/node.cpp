#include "node.h"
#include <iostream>
#include <iomanip>
#include <sstream>


Node::Node(SyntaxKind k): kind(k) {
}

OperatorExpr::OperatorExpr(Token::Pointer token):
Node(SyntaxKind::operatorExpr),
token(token) {
    
}

ConstDecl::ConstDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer):
Node(SyntaxKind::constantDecl),
pattern(pattern),
initializer(initializer) {
}

VarDecl::VarDecl(Pattern::Pointer pattern, std::shared_ptr<Node> initializer):
Node(SyntaxKind::varDecl),
pattern(pattern),
initializer(initializer) {
}

ClassDecl::ClassDecl(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Node>> members):
Node(SyntaxKind::classDecl),
name(name),
members(members) {
}

ParameterClause::ParameterClause(std::vector<std::shared_ptr<Pattern>> parameters):
Node(SyntaxKind::parameterClause),
parameters(parameters) {
}

FuncDecl::FuncDecl(Node::Pointer identifier, Node::Pointer parameterClause, Node::Pointer returnType, Node::Pointer codeBlock):
Node(SyntaxKind::funcDecl),
identifier(identifier),
parameterClause(parameterClause),
returnType(returnType),
codeBlock(codeBlock) {
}

const std::wstring FuncDecl::getFuncName() {
    std::wstringstream ss;
    ss << std::static_pointer_cast<IdentifierExpr>(identifier)->token->rawValue << L"(";
    for(auto p: std::static_pointer_cast<ParameterClause>(parameterClause)->parameters) {
        ss << p->identifier->token->rawValue << L":";
    }
    ss << L")";
    return ss.str();
}

Pattern::Pattern(IdentifierExpr::Pointer identifier, TypeDecl::Pointer type):
Node(SyntaxKind::pattern),
identifier(identifier),
type(type) {
}


ConstructorDecl::ConstructorDecl(std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock):
Node(SyntaxKind::constructorDecl),
parameterClause(parameterClause),
codeBlock(codeBlock) {
}

Expr::Expr(Node::Pointer prefix, std::vector<Node::Pointer> binaries):
Node(SyntaxKind::expr),
prefix(prefix),
binaries(binaries) {
}

Expr::Expr(std::vector<Node::Pointer> nodes):
Node(SyntaxKind::expr),
prefix(nullptr),
binaries(),
nodes(nodes){
    
}

PostfixExpr::PostfixExpr(Node::Pointer expr, OperatorExpr::Pointer op):
Node(SyntaxKind::postfixExpr),
expr(expr),
op(op) {
}

PrefixExpr::PrefixExpr(OperatorExpr::Pointer op, Node::Pointer expr):
Node(SyntaxKind::prefixExpr),
op(op),
expr(expr) {
    
}

BinaryExpr::BinaryExpr(OperatorExpr::Pointer op, Node::Pointer expr):
Node(SyntaxKind::binaryExpr),
op(op),
expr(expr) {
}

AssignmentExpr::AssignmentExpr(Node::Pointer expr):
Node(SyntaxKind::assignmentExpr),
expr(expr) {
}

ArguCallExpr::ArguCallExpr(IdentifierExpr::Pointer identifier, Node::Pointer expr):
Node(SyntaxKind::arguCallExpr),
label(identifier),
expr(expr) {
}

FuncCallExpr::FuncCallExpr(IdentifierExpr::Pointer identifier, std::vector<ArguCallExpr::Pointer> arguments):
Node(SyntaxKind::functionCallExpr),
identifier(identifier),
arguments(arguments) {
}

MemberExpr::MemberExpr(std::shared_ptr<Node> parent, std::shared_ptr<Node> member):
Node(SyntaxKind::memberExpr),
parent(parent),
member(member) {
}

LiteralExpr::LiteralExpr(std::shared_ptr<Token> literal):
Node(SyntaxKind::literalExpr),
literal(literal) {
    
}

IdentifierExpr::IdentifierExpr(std::shared_ptr<Token> token):
Node(SyntaxKind::identifierExpr),
token(token) {
}

ParenthesizedExpr::ParenthesizedExpr(std::shared_ptr<Node> expr):
Node(SyntaxKind::parenthesizedExpr),
expr(expr) {
}

SelfExpr::SelfExpr(IdentifierExpr::Pointer identifier):
Node(SyntaxKind::selfExpr),
identifier(identifier) {
}

TypeDecl::TypeDecl(IdentifierExpr::Pointer identifier, bool isOptional):
Node(SyntaxKind::type),
identifier(identifier),
isOptional(isOptional) {
}

SourceBlock::SourceBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::sourceBlock),
statements(statements) {

}

CodeBlock::CodeBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::codeBlock),
statements(statements) {
}

ForInStatement::ForInStatement(std::shared_ptr<Node> pattern, std::shared_ptr<Node> inExpr, std::shared_ptr<Node> codeBlock):
Node(SyntaxKind::forInStatement),
pattern(pattern),
inExpr(inExpr),
codeBlock(codeBlock) {
}

IfStatement::IfStatement(Node::Pointer condition, Node::Pointer ifCodeBlock, std::shared_ptr<Node> elseCodeBlock):
Node(SyntaxKind::ifStatement),
condition(condition),
ifCodeBlock(ifCodeBlock),
elseCodeBlock(elseCodeBlock) {
}

ReturnStatement::ReturnStatement(Node::Pointer expr):
Node(SyntaxKind::returnStatement),
expr(expr) {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NodeDebugPrinter::print(std::vector<Node::Pointer> nodes) {
    std::wcout << std::endl;
    printTab();
    
    std::wcout << L"[";
    incTab();
    
    for(auto n : nodes) {
        print(n);
    }
    decTab();
    std::wcout << std::endl;
    printTab();
    
    std::wcout << L"]";
    
}

void NodeDebugPrinter::print(Node::Pointer node) {
    if(node == nullptr) {
        return;
    }
    if (size > 0 ) {
        std::wcout << std::endl;
    }
    printTab();
    
    switch (node->kind) {
        case sourceBlock: {
            std::wcout << L"+sourceBLock" ;
            auto n = std::static_pointer_cast<SourceBlock>(node);
            incTab();
            for(auto statement: n->statements) {
                print(statement);
            }
            decTab();
        }
            break;
        case type: {
            std::wcout << L"+type" ;
            auto n = std::static_pointer_cast<TypeDecl>(node);
            incTab();
            print(n->identifier);
            decTab();
        }
            break;
        case arrayType:
            std::wcout << L"+arrayType" ;
            break;
        case dictType:
            std::wcout << L"+dictType" ;
            break;
        case pattern: {
            std::wcout << L"+pattern" ;
            auto n = std::static_pointer_cast<Pattern>(node);
            incTab();
            print(n->identifier);
            if(n->type != nullptr) {
                print(n->type);
            }
            decTab();
        }
            break;
        case importDecl:
            std::wcout << L"+importDecl" ;
            break;
        case constantDecl: {
            std::wcout << L"+constantDecl" ;
            auto n = std::static_pointer_cast<ConstDecl>(node);
            incTab();
            print(n->pattern);
            print(n->initializer);
            decTab();
        }
            
            break;
        case varDecl: {
            std::wcout << L"+varDecl" ;
            auto n = std::static_pointer_cast<VarDecl>(node);
            incTab();
            print(n->pattern);
            print(n->initializer);
            decTab();
        }
            break;
        case letDecl:
            std::wcout << L"+letDecl" ;
            break;
        case funcDecl: {
            std::wcout << L"+funcDecl" ;
            auto n = std::static_pointer_cast<FuncDecl>(node);
            incTab();
            print(n->identifier);
            print(n->parameterClause);
            print(n->returnType);
            print(n->codeBlock);
            decTab();
        }
            break;
        case constructorDecl:
            std::wcout << L"+constructorDecl" ;
            break;
        case classDecl:
            std::wcout << L"+classDecl" ;
            break;
        case parameterClause: {
            std::wcout << L"+parameterClause" ;
            auto n = std::static_pointer_cast<ParameterClause>(node);
            incTab();
            std::vector<Node::Pointer> parameters;
            for(auto p: n->parameters) {
                parameters.push_back(p);
            }
            print(parameters);
            decTab();
        }
            break;
        case codeBlock: {
            auto n = std::static_pointer_cast<CodeBlock>(node);
            std::wcout << L"+codeBlock" ;
            incTab();
            print(n->statements);
            decTab();
        }
            break;
        case forInStatement:
            std::wcout << L"+forInStatement" ;
            break;
        case ifStatement: {
            auto n = std::static_pointer_cast<IfStatement>(node);
            std::wcout << L"+ifStatement" ;
            incTab();
            print(n->condition);
            print(n->ifCodeBlock);
            print(n->elseCodeBlock);
            decTab();
        }
            break;
        case expr: {
            std::wcout << L"+expr" ;
            auto n = std::static_pointer_cast<Expr>(node);
            incTab();
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
            std::wcout << L"+selfExpr" ;
            break;
        case postfixExpr: {
            std::wcout << L"+postfixExpr" ;
            auto n = std::static_pointer_cast<PostfixExpr>(node);
            incTab();
            print(n->expr);
            print(n->op);
            decTab();
        }
            break;
        case prefixExpr: {
            std::wcout << L"+prefixExpr" ;
            auto n = std::static_pointer_cast<PrefixExpr>(node);
            incTab();
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
            std::wcout << L"+identifierExpr(" << n->token->rawValue << ")"; ;
            
        }
            break;
        case parenthesizedExpr: {
            auto n = std::static_pointer_cast<ParenthesizedExpr>(node);
            std::wcout << L"+parenthesizedExpr" ;
            incTab();
            print(n->expr);
            decTab();
        }
            break;
        case arguCallExpr: {
            std::wcout << L"+arguCallExpr" ;
            auto n = std::static_pointer_cast<ArguCallExpr>(node);
            incTab();
            print(n->label);
            print(n->expr);
            decTab();
        }
            break;
        case functionCallExpr: {
            std::wcout << L"+functionCallExpr" ;
            auto n = std::static_pointer_cast<FuncCallExpr>(node);
            incTab();
            print(n->identifier);
            for(auto argu: n->arguments) {
                print(argu);
            }
            decTab();
        }
            break;
        case memberExpr:
            std::wcout << L"+memberExpr" ;
            break;
        case literalExpr: {
            auto n = std::static_pointer_cast<LiteralExpr>(node);
            std::wcout << L"+literalExpr(" << n->literal->rawValue << ")" ;
        }
            break;
        case arrayLiteralExpr:
            std::wcout << L"+arrayLiteralExpr" ;
            break;
        case dictLiteralExpr:
            std::wcout << L"+dictLiteralExpr" ;
            break;
        case assignmentExpr: {
            std::wcout << L"+assignmentExpr" ;
            auto n = std::static_pointer_cast<AssignmentExpr>(node);
            incTab();
            print(n->identifier);
            print(n->expr);
            decTab();
        }
            
            break;
        case binaryExpr: {
            std::wcout << L"+binaryExpr" ;
            auto n = std::static_pointer_cast<BinaryExpr>(node);
            
            incTab();
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
            std::wcout << L"+operatorExpr(" << n->token->rawValue << ")" ;
        }
            break;
        case returnStatement: {
            auto n = std::static_pointer_cast<ReturnStatement>(node);
            std::wcout << L"+returnStatement";
            incTab();
            print(n->expr);
            decTab();
        }
            break;
    }

}

void NodeDebugPrinter::printTab() {
    std::wcout << std::wstring(size*2, L' ') ;
}

void NodeDebugPrinter::incTab() {
    size ++;
}

void NodeDebugPrinter::decTab() {
    size --;
}
