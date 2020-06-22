#include "binder.h"

/////////////////////////////////////////////////////////////////
// BindContext
/////////////////////////////////////////////////////////////////

SymTable::Pointer BindContext::currentSymTable() {
    return symbols.top();
}

Scope::Pointer BindContext::currentScope() {
    return scopes.top();
}

void BindContext::enter(SymTable::Pointer table) {
    symbols.push(table);
}

void BindContext::leave(SymTable::Pointer table) {
    // TODO: check the top is the table
    symbols.pop();
}

void BindContext::enterScope(ScopeFlag flag, TypeDescriptor::Pointer scopeType) {
    scopes.push(std::make_shared<Scope>(flag, scopeType));
}
 
void BindContext::leaveScope(ScopeFlag flag, TypeDescriptor::Pointer scopeType) {
    scopes.pop();
}

Symbol::Pointer BindContext::makeSymbol(Node::Pointer node, const std::wstring &name, SymbolFlag flag) {
    return std::make_shared<Symbol>(flag, name);
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
            bind(std::static_pointer_cast<VarDecl>(node));
            break;
        case letDecl:
            break;
        case funcDecl:
            break;
        case constructorDecl:
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
    SourceFileTypeDescriptor::Pointer sourceFileType = typeFactory->createSourceFileType(L"sourcefile");
    
    context->enterScope(ScopeFlag::sourceScope, sourceFileType);
    context->enter(symtable);
    
    for(auto& decl : sourceBlock->decls) {
        bind(decl);
    }
    
    context->leave(symtable);
    context->leaveScope(ScopeFlag::sourceScope, sourceFileType);
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
    
    // go down to parse Class members
    context->enterScope(ScopeFlag::classScope, classType);
    context->enter(symtable);
    
    for(auto& member: classDecl->members) {
        bind(member);
    }
    context->leave(symtable);
    context->leaveScope(ScopeFlag::classScope, classType);
}

void Binder::bind(VarDecl::Pointer decl) {
    SymTable::Pointer symtable = context->currentSymTable();
    Pattern::Pointer pattern = decl->pattern;
    
    Symbol::Pointer symbol = context->makeSymbol(decl, pattern->identifier->value, SymbolFlag::varSymbol);
    
    switch (context->currentScope()->flog) {
        case sourceScope:
            break;
        case classScope: {
            ClassTypeDescriptor::Pointer owenerClassType = std::static_pointer_cast<ClassTypeDescriptor>(context->currentScope()->scopeType);
            FieldTypeDescriptor::Pointer fieldType = typeFactory->createFieldType(pattern->identifier->value);
            
            // go down
            context->enterScope(ScopeFlag::fie, <#TypeDescriptor::Pointer scopeType#>)
        }
            break;
        default:
            break;
    }
}
