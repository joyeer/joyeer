#include "IRGen.h"

std::vector<Instruction>& IRGen::getInstructions() {
    return writer.instructions;
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
            break;
        case varDecl:
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
            break;
        case identifierExpr:
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
