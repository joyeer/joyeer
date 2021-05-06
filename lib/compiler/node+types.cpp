#include "joyeer/compiler/node+types.h"
#include <fstream>
#include <sstream>

FileModuleNode::FileModuleNode(FileModuleDescriptor::Ptr descriptor, std::vector<std::shared_ptr<Node>> statements):
DeclNode(SyntaxKind::sourceBlock),
statements(statements) {
    this->descriptor = descriptor;
}

std::string FileModuleNode::getName() {
    std::filesystem::path p = filename;
    return p.replace_extension().string();
}

std::vector<FileImportStatement::Ptr> FileModuleNode::getFileImports() {
    std::vector<FileImportStatement::Ptr> result;
    for(std::vector<Node::Ptr>::const_iterator iterator = statements.begin(); iterator != statements.end(); iterator ++ ) {
        auto node = *iterator;
        if(node->kind == SyntaxKind::fileimportStatement) {
            result.push_back(std::static_pointer_cast<FileImportStatement>(node));
        }
    }
    return result;
}

/// Function Decl declaration
FuncDecl::FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock):
DeclNode(SyntaxKind::funcDecl),
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


/// Class Constructor declaration
ConstructorDecl::ConstructorDecl(std::shared_ptr<Node> parameterClause, std::shared_ptr<Node> codeBlock):
DeclNode(SyntaxKind::constructorDecl),
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

ClassDecl::ClassDecl(Token::Ptr name, std::vector<Node::Ptr> members):
DeclNode(SyntaxKind::classDecl),
name(name),
members(members) {
}

std::string ClassDecl::getName() {
    return name->rawValue;
}

VarDecl::VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer):
DeclNode(SyntaxKind::varDecl),
pattern(pattern),
initializer(initializer) {
}
