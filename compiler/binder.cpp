#include "binder.h"
#include "diagnostic.h"
#include "runtime/buildin.h"

/////////////////////////////////////////////////////////////////
// BindContext
/////////////////////////////////////////////////////////////////

BindContext::BindContext(SymbolFactory::Pointer factory):
factory(factory) {
    symbols.push_back(factory->getGlobalSymbolTable());
}

Symbol::Pointer BindContext::findSymbol(const std::wstring &name) {
    for(auto symtable : symbols) {
        auto result = symtable->find(name);
        if(result != nullptr) {
            return result;
        }
    }
    return nullptr;
}

SymTable::Pointer BindContext::currentSymTable() {
    return *symbols.end();
}

Scope::Pointer BindContext::currentScope() {
    return scopes.top();
}

void BindContext::enter(SymTable::Pointer table) {
    symbols.push_back(table);
}

void BindContext::leave(SymTable::Pointer table) {
    // TODO: check the top is the table
    symbols.back();
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
context(std::make_shared<BindContext>(symFactory)) {
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
            bind(std::static_pointer_cast<ConstructorDecl>(node));
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
            bind(std::static_pointer_cast<ArguCallExpr>(node));
            break;
        case functionCallExpr:
            bind(std::static_pointer_cast<FuncCallExpr>(node));
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
    SymTable::Pointer symtable = symFactory->createSymTable();
    SourceFileTypeDescriptor::Pointer sourceFileType = typeFactory->createSourceFileType(L"sourcefile");
    
    context->enterScope(ScopeFlag::sourceScope, sourceFileType);
    context->enter(symtable);
    
    for(auto& statement : sourceBlock->statements) {
        bind(statement);
    }
    
    context->leave(symtable);
    context->leaveScope(ScopeFlag::sourceScope, sourceFileType);
}

void Binder::bind(ClassDecl::Pointer classDecl) {
    
    // TODO: double the parent scope is Source File
    
    SourceFileTypeDescriptor::Pointer sourceFile = std::static_pointer_cast<SourceFileTypeDescriptor>(context->currentScope()->scopeType);
    
    
    const std::wstring& className = classDecl->name->value;
    // declara an new class type
    ClassTypeDescriptor::Pointer classType = typeFactory->createClassType(className);
    
    // delcara an class Symbol
    Symbol::Pointer classDeclSymbol = std::make_shared<Symbol>(SymbolFlag::classTypeSymbol, className);
    
    context->currentSymTable()->insert(classDeclSymbol);
    
    SymTable::Pointer symtable = symFactory->createSymTable();
    
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
            ClassTypeDescriptor::Pointer ownerClassType = std::static_pointer_cast<ClassTypeDescriptor>(context->currentScope()->scopeType);
            FieldTypeDescriptor::Pointer fieldType = typeFactory->createFieldType(pattern->identifier->value);
            
            ownerClassType->append(fieldType);
            
            // go down
            context->enterScope(ScopeFlag::fieldScope, fieldType);
            
            context->leaveScope(ScopeFlag::fieldScope, fieldType);
        }
            break;
        default:
            break;
    }
}

void Binder::bind(ConstructorDecl::Pointer decl) {
    Scope::Pointer scope = context->currentScope();
    
    // TODO: check the constructor must be in Class scope
    ClassTypeDescriptor::Pointer ownerClassType = std::static_pointer_cast<ClassTypeDescriptor>(context->currentScope()->scopeType);
    MethodTypeDescriptor::Pointer methodType = typeFactory->createMethodType(L"constructor");
}

void Binder::bind(FuncCallExpr::Pointer decl) {
    // go down to bind argument
    for(auto& paramter: decl->arguments) {
        bind(paramter);
    }
    
    std::wstring name = decl->identifier->value;
    name += L"(";
    for(auto& argument: decl->arguments) {
        name += argument->label->value;
        name += L":";
    }
    name += L")";
    
    auto symbol = context->findSymbol(name);
    if(symbol == nullptr) {
        Diagnostics::reportError(L"Cannot find function");
    }
    
    decl->symbol = symbol;
}

void Binder::bind(ArguCallExpr::Pointer decl) {
    bind(decl->expr);
}

void Binder::bind(LiteralExpr::Pointer decl) {
    
}
