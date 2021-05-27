#include "joyeer/compiler/node+types.h"
#include <fstream>
#include <sstream>

ClassDecl::ClassDecl(Token::Ptr name, std::vector<Node::Ptr> members):
DeclNode(SyntaxKind::classDecl),
name(name),
members(members),
staticFields(),
staticMethods(),
instanceMethods(),
instanceFields() {
    symtable = std::make_shared<SymbolTable>();
}

std::string ClassDecl::getSimpleName() {
    return name->rawValue;
}

FileModuleDecl::FileModuleDecl(FileModuleDescriptor::Ptr descriptor, StmtsBlock::Ptr block):
ClassDecl(nullptr, block->statements),
block(block) {
    kind = SyntaxKind::filemodule;
    this->descriptor = descriptor;
}

std::string FileModuleDecl::getSimpleName() {
    std::filesystem::path p = filename;
    return p.filename().replace_extension().string();
}

/// Function Decl declaration
FuncDecl::FuncDecl(Node::Ptr identifier, Node::Ptr parameterClause, Node::Ptr returnType, Node::Ptr codeBlock):
DeclNode(SyntaxKind::funcDecl),
identifier(identifier),
parameterClause(parameterClause),
returnType(returnType),
codeBlock(codeBlock) {
    symtable = std::make_shared<SymbolTable>();
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

std::string FuncDecl::getSimpleName() {
    std::stringstream ss;
    if(identifier != nullptr) {
        ss << identifier->getSimpleName();
    } else if(isConstructor) {
        auto declaringClassDecl = getDeclaringClassDecl();
        ss << declaringClassDecl->getSimpleName();
    }
    
    ss << DescriptorConstants::ParenthesisOpen;
    for(auto p : std::static_pointer_cast<ParameterClause>(parameterClause)->parameters) {
        ss << p->getSimpleName() << DescriptorConstants::Colon;
    }
    ss << DescriptorConstants::ParenthesisClose;
    
    return ss.str();
}

VarDecl::VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer):
DeclNode(SyntaxKind::varDecl),
pattern(pattern),
initializer(initializer) {
}
