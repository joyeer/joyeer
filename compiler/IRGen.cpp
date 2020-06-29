#include "IRGen.h"
#include "diagnostic.h"
#include "runtime/buildin.h"
#include <cassert>

IRGen::IRGen() {
    function = std::make_shared<JrFunction>();
}

std::vector<Instruction>& IRGen::getInstructions() {
    return writer.instructions;
}

JrFunction::Pointer IRGen::getFunction() {
    function->instructions = writer.instructions;
    Global::registerFunction(function);
    
    return function;
}

void IRGen::emit(Node::Pointer node) {
    switch (node->kind) {
        case sourceBlock:
            emit(std::static_pointer_cast<SourceBlock>(node));
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
            emit(std::static_pointer_cast<ConstDecl>(node));
            break;
        case varDecl:
            emit(std::static_pointer_cast<VarDecl>(node));
            break;
        case letDecl:
            break;
        case funcDecl:
            break;
        case constructorDecl:
            break;
        case classDecl:
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
            emit(std::static_pointer_cast<PrefixExpr>(node));
            break;
        case identifierExpr:
            emit(std::static_pointer_cast<IdentifierExpr>(node));
            break;
        case parenthesizedExpr:
            break;
        case arguCallExpr:
            emit(std::static_pointer_cast<ArguCallExpr>(node));
            break;
        case functionCallExpr:
            emit(std::static_pointer_cast<FuncCallExpr>(node));
            break;
        case memberExpr:
            break;
        case literalExpr:
            emit(std::static_pointer_cast<LiteralExpr>(node));
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

void IRGen::emit(SourceBlock::Pointer block) {
    
    //initialize the variables for source code scope
    varFinder.scopes.push_back(block->scope);
    for(auto var: block->scope->vars) {
        function->localVars.push_back(JrVar {
            .name = var->name
        });
    }
    
    for(auto& statement: block->statements) {
        emit(statement);
    }
}

void IRGen::emit(FuncCallExpr::Pointer funcCallExpr) {
    for(auto argument : funcCallExpr->arguments) {
        emit(argument);
    }
    
    auto funcIndex = funcCallExpr->symbol->index;
    writer.write({
        .opcode = OP_INVOKE,
        .value = (int32_t)funcIndex
    });
}

void IRGen::emit(ArguCallExpr::Pointer node) {
    emit(node->expr);
}

void IRGen::emit(LiteralExpr::Pointer node) {
    switch(node->literal->kind) {
        case decimalLiteral:
            writer.write({
                .opcode = OP_ICONST,
                .value = node->literal->intValue
            });
            break;
        case stringLiteral:
            writer.write({
                .opcode = OP_SCONST,
                .value = node->literal->index
            });
        default:
            break;;
    }
}

void IRGen::emit(ConstDecl::Pointer node) {
    emit(node->initializer);
    
    // TODO: detect the variable's type
    writer.write({
        .opcode = OP_ISTORE,
        .value = node->variable->index
    });
    
}

void IRGen::emit(VarDecl::Pointer node) {
    emit(node->initializer);
    
    // TODO: detect the variable's type
    writer.write({
        .opcode = OP_ISTORE,
        .value = node->variable->index
    });
}

void IRGen::emit(PrefixExpr::Pointer node) {
    assert(node->expr != nullptr);
    emit(node->expr);
}

void IRGen::emit(IdentifierExpr::Pointer node) {
    // TODO:
    // Step1: try to find the variable in local variable array
    
    auto name = node->identifier->rawValue;
    auto variable = varFinder.find(name);
    if(variable == nullptr) {
        Diagnostics::reportError(L"[Error][GenCode]");
    }
    writer.write({
        .opcode = OP_ILOAD,
        .value = variable->index
    });
    
    // Step2: try to find the symbol in symbols
}
