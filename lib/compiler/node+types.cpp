#include "joyeer/compiler/node+types.h"
#include <fstream>
#include <sstream>

ClassDecl::ClassDecl(Token::Ptr name, StmtsBlock::Ptr members):
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
ClassDecl(nullptr, block) {
    this->members = block;
    kind = SyntaxKind::fileModule;
    this->descriptor = descriptor;
    assert(symtable != nullptr);
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

std::string FuncDecl::getSimpleName() {
    std::stringstream ss;
    
    // basis name
    if(identifier != nullptr) {
        ss << identifier->getSimpleName();
    } else if(type == FuncType::constructor || type == FuncType::staticInitializer) {
        auto declaringClassDecl = getDeclaringClassDecl();
        ss << declaringClassDecl->getSimpleName();
    }
    
    // parameters
    ss << DescriptorConstants::ParenthesisOpen;
    if(parameterClause) {
        for(auto p : std::static_pointer_cast<ParameterClause>(parameterClause)->parameters) {
            ss << p->getSimpleName() << DescriptorConstants::Colon;
        }
    }
    ss << DescriptorConstants::ParenthesisClose;
    
    return ss.str();
}

VarDecl::VarDecl(Pattern::Ptr pattern, std::shared_ptr<Node> initializer):
DeclNode(SyntaxKind::varDecl),
pattern(pattern),
initializer(initializer) {
}
