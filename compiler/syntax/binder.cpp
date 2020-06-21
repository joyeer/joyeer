#include "binder.h"

/////////////////////////////////////////////////////////////////
// BindContext
/////////////////////////////////////////////////////////////////

SymTable::Pointer BindContext::currentSymTable() {
    return symbols.top();
}

void BindContext::enter(SymTable::Pointer table) {
    symbols.push(table);
}

void BindContext::leave(SymTable::Pointer table) {
    // TODO: check the top is the table
    symbols.top();
}

/////////////////////////////////////////////////////////////////
// Binder
/////////////////////////////////////////////////////////////////

Binder::Binder(SymbolFactory::Pointer symFactory, TypeFactory::Pointer typeFactory):
symFactory(symFactory),
typeFactory(typeFactory),
context(std::make_shared<BindContext>()){
}

void Binder::bind(std::shared_ptr<Node> node) {
    switch (node->kind) {
        case SyntaxKind::sourceBlock:
            bind(std::static_pointer_cast<SourceBlock>(node));
            break;
        case type:
            break;
        case arrayType:
            break;
        case dictType:
            break;
        case pattern:
            break;
        case importDecl:
            break;
        case constantDecl:
            break;
        case varDecl:
            break;
        case letDecl:
            break;
        case funcDecl:
            break;
        case initializerDecl:
            break;
        case classDecl:
            bind(std::static_pointer_cast<ClassDecl>(node));
            break;
        case parameterClause:
            break;
        case codeBlock:
            break;
        case forInStatement:
            break;
        case ifStatement:
            break;
        case expr:
            break;
        case selfExpr:
            break;
        case postfixExpr:
            break;
        case prefixExpr:
            break;
        case identifierExpr:
            break;
        case parenthesizedExpr:
            break;
        case arguCallExpr:
            break;
        case functionCallExpr:
            break;
        case memberExpr:
            break;
        case literalExpr:
            break;
        case arrayLiteralExpr:
            break;
        case dictLiteralExpr:
            break;
        case assignmentOperator:
            break;
        case binaryOperator:
            break;
    }
}


void Binder::bind(SourceBlock::Pointer sourceBlock) {
    SymTable::Pointer symtable = symFactory->createSymTable(sourceBlock);
    
    context->enter(symtable);
    
    for(auto& decl : sourceBlock->decls) {
        bind(decl);
    }
    
    context->leave(symtable);
}

void Binder::bind(ClassDecl::Pointer classDecl) {
    const std::wstring& className = classDecl->name->value;
    // declara an new class type
    ClassTypeDescriptor::Pointer classType = typeFactory->createClassType(className);
    
    // delcara an class Symbol
    Symbol::Pointer classDeclSymbol = std::make_shared<Symbol>(SymbolFlag::classTypeSymbol, className);
    classDeclSymbol->type = std::static_pointer_cast<TypeDescriptor>(classType);
    
    context->currentSymTable()->insert(classDeclSymbol);
    
    SymTable::Pointer symtable = symFactory->createSymTable(classDecl);
    
    context->enter(symtable);
    for(auto& member: classDecl->members) {
        bind(member);
    }
    context->leave(symtable);
}

void Binder::bind(VarDecl::Pointer varDecl) {
    
}
