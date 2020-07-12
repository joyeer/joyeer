#include "IRGen.h"
#include "diagnostic.h"
#include "runtime/buildin.h"
#include <cassert>
#include <unordered_map>


IRGen::IRGen(CompileContext::Pointer context):
context(context) {
    
}

std::vector<Instruction>& IRGen::getInstructions() {
    return writer.instructions;
}

JrFunction::Pointer IRGen::getFunction() {
    func->instructions = getInstructions();
    return func;
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
        case letDecl:
            emit(std::static_pointer_cast<LetDecl>(node));
            break;
        case varDecl:
            emit(std::static_pointer_cast<VarDecl>(node));
            break;
        case funcDecl:
            emit(std::static_pointer_cast<FuncDecl>(node));
            break;
        case constructorDecl:
            break;
        case classDecl:
            break;
        case parameterClause:
            break;
        case codeBlock:
            emit(std::static_pointer_cast<CodeBlock>(node));
            break;
        case forInStatement:
            break;
        case ifStatement:
            emit(std::static_pointer_cast<IfStatement>(node));
            break;
        case expr:
            emit(std::static_pointer_cast<Expr>(node));
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
            emit(std::static_pointer_cast<ParenthesizedExpr>(node));
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
        case assignmentExpr:
            emit(std::static_pointer_cast<AssignmentExpr>(node));
            break;
        case binaryExpr:
            break;
        case operatorExpr:
            emit(std::static_pointer_cast<OperatorExpr>(node));
            break;
        case returnStatement:
            emit(std::static_pointer_cast<ReturnStatement>(node));
            break;
    }
}

void IRGen::emit(SourceBlock::Pointer block) {
    
    assert(block->symbol->flag == funcSymbol);
    func = Global::functions[block->symbol->addressOfFunc];
    
    context->visit(visitSourceBlock, [this, block]() {
        for(auto& statement: block->statements) {
            emit(statement);
        }
    });
    
}

void IRGen::emit(FuncCallExpr::Pointer funcCallExpr) {
    for(auto argument : funcCallExpr->arguments) {
        emit(argument);
    }
    
    auto funcIndex = funcCallExpr->symbol->addressOfFunc;
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
            break;
        case booleanLiteral:
            writer.write({
                .opcode = OP_ICONST,
                .value = node->literal->rawValue == L"true" ? 1 : 0
            });
            break;
        default:
            break;;
    }
}

void IRGen::emit(LetDecl::Pointer node) {
    emit(node->initializer);
    
    // TODO: detect the variable's type
    writer.write({
        .opcode = OP_ISTORE,
        .value = node->symbol->index
    });
    
}

void IRGen::emit(VarDecl::Pointer node) {
    emit(node->initializer);
    
    // TODO: detect the variable's type
    writer.write({
        .opcode = OP_ISTORE,
        .value = node->symbol->index
    });
}

void IRGen::emit(PrefixExpr::Pointer node) {
    assert(node->expr != nullptr);
    emit(node->expr);
}

void IRGen::emit(IdentifierExpr::Pointer node) {
    auto symbol = node->symbol;
    if(symbol == nullptr) {
        Diagnostics::reportError(L"[Error][GenCode]");
        return;
    }
    
    if((symbol->flag & varSymbol) == varSymbol) {
        writer.write({
            .opcode = OP_ILOAD,
            .value = symbol->addressOfVariable
        });
        
        return;
    }

    return;
    // Step2: try to find the symbol in symbols
}

void IRGen::emit(AssignmentExpr::Pointer node) {
    emit(node->expr);
    writer.write({
        .opcode = OP_ISTORE,
        .value = node->identifier->symbol->index
    });
}

void IRGen::emit(Expr::Pointer node) {
    assert(node->prefix == nullptr);
    assert(node->binaries.size() == 0);
    
    for(auto n: node->nodes) {
        emit(n);
    }
}

void IRGen::emit(OperatorExpr::Pointer node) {
    
    static std::unordered_map<std::wstring, Opcode> imaps {
        {
            { Operators::MINUS, OP_ISUB },
            { Operators::PLUS, OP_IADD },
            { Operators::DIV, OP_IDIV },
            { Operators::MULTIPLY, OP_IMUL },
            { Operators::PERCENTAGE, OP_IREM }
        }
    };
    
    writer.write({
        .opcode = imaps[node->token->rawValue]
    });
}

void IRGen::emit(ParenthesizedExpr::Pointer node) {
    emit(node->expr);
}

void IRGen::emit(IfStatement::Pointer node) {
    IRGen gen(context);
    gen.emit(node->ifCodeBlock);
    auto instructions = gen.getInstructions();
    
    std::vector<Instruction> elseInstructions;
    if(node->elseCodeBlock != nullptr) {
        IRGen elseBlockGenerator(context);
        elseBlockGenerator.emit(node->elseCodeBlock);
        elseInstructions = elseBlockGenerator.getInstructions();
        
        // insert an goto instruction in ifInstructions;
        instructions.push_back({
            .opcode = OP_GOTO,
            .value = static_cast<int32_t>(elseInstructions.size())
        });
    }
    
    emit(node->condition);
    writer.write({
        .opcode = OP_IFLE,
        .value = static_cast<int32_t>(instructions.size())
    });
    writer.write(instructions);
    writer.write(elseInstructions);
    
}

void IRGen::emit(CodeBlock::Pointer node) {
    context->visit(visitCodeBlock, [this, node]() {
        for(auto statement: node->statements) {
            emit(statement);
        }
    });
}

void IRGen::emit(FuncDecl::Pointer node) {
    
    IRGen generator(context);
    generator.emit(node->codeBlock);
    auto instructions = generator.getInstructions();
    
    assert(node->symbol->flag == funcSymbol);
    auto function = Global::functions[node->symbol->addressOfFunc];
    assert(function != nullptr && function->instructions.size() == 0);
    function->instructions = instructions;
    
    auto symbols = node->symtable->allVarSymbols();
    
    for(auto symbol: symbols) {
        symbol->index = function->localVars.size();
        function->localVars.push_back(JrVar {
            .name = symbol->name
        });
    }


}

void IRGen::emit(ReturnStatement::Pointer node) {
    Opcode op = OP_IRETURN;
    if(node->expr != nullptr) {
        emit(node->expr);
        op = OP_IRETURN;
        
    }
    writer.write({
        .opcode = op
    });

}
