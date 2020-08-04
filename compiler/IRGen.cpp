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
            emit(std::static_pointer_cast<ConstructorDecl>(node));
            break;
        case classDecl:
            emit(std::static_pointer_cast<ClassDecl>(node));
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
        case funcCallExpr:
            emit(std::static_pointer_cast<FuncCallExpr>(node));
            break;
        case memberAccessExpr:
            emit(std::static_pointer_cast<MemberAccessExpr>(node));
            break;
        case literalExpr:
            emit(std::static_pointer_cast<LiteralExpr>(node));
            break;
        case arrayLiteralExpr:
            emit(std::static_pointer_cast<ArrayLiteralExpr>(node));
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
        case subscriptExpr:
            emit(std::static_pointer_cast<SubscriptExpr>(node));
            break;
        default:
            assert(false);
    }
}

void IRGen::emit(SourceBlock::Pointer block) {
    
    assert(block->symbol->flag == moduleSymbol);
    auto module = std::static_pointer_cast<JrModuleType>(Global::types[block->symbol->addressOfType]);
    assert(module->constructors.size() == 1);
    func = Global::functions[module->constructors.back()];
    
    context->visit(visitSourceBlock, [this, block]() {
        for(auto& statement: block->statements) {
            emit(statement);
        }
    });
    
    func->instructions = getInstructions();
}

void IRGen::emit(FuncCallExpr::Pointer funcCallExpr) {
    for(auto argument : funcCallExpr->arguments) {
        emit(argument);
    }
    
    auto function = Global::functions[funcCallExpr->symbol->addressOfFunc];
    assert(function != nullptr);
    if((function->kind & jrFuncConstructor) == jrFuncConstructor) {
        writer.write({
            .opcode = OP_NEW,
            .value = (int32_t)function->addressOfFunc
        });
    } else {
        writer.write({
            .opcode = OP_INVOKE,
            .value = (int32_t)function->addressOfFunc
        });
    }
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
        case nilLiteral:
            writer.write({
                .opcode = OP_OCONST_NIL
            });
            break;
        default:
            assert(false);
            break;
    }
}

void IRGen::emit(LetDecl::Pointer node) {
    emit(node->initializer);
    
    // TODO: detect the variable's type
    writer.write({
        .opcode = OP_ISTORE,
        .value = node->symbol->addressOfVariable
    });
    
}

void IRGen::emit(VarDecl::Pointer node) {
    emit(node->initializer);
    
    // TODO: detect the variable's type
    writer.write({
        .opcode = OP_ISTORE,
        .value = node->symbol->addressOfVariable
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
        auto type = Global::types[symbol->addressOfType];
        
        if(type == JrPrimaryType::Int) {
            writer.write({
                .opcode = OP_ILOAD,
                .value = symbol->addressOfVariable
            });
        } else if(type->kind == JrType_Object || type->kind == JrType_Any ){
            writer.write({
                .opcode = OP_OLOAD,
                .value = symbol->addressOfVariable
            });
        } else {
            assert(false);
        }
        
        return;
    }

    assert(false);
    // Step2: try to find the symbol in symbols
}

void IRGen::emit(AssignmentExpr::Pointer node) {
    emit(node->expr);
    if(node->left->kind == identifierExpr) {
        auto identifierExpr = std::static_pointer_cast<IdentifierExpr>(node->left);
        writer.write({
            .opcode = OP_ISTORE,
            .value = identifierExpr->symbol->addressOfVariable
        });
    } else if( node->left->kind == selfExpr ) {
        auto selfExpr = std::static_pointer_cast<SelfExpr>(node->left);
        
        auto function = context->curFunction();
        
        writer.write({
            .opcode = OP_OLOAD,
            .value = (int32_t)(function->paramTypes.size() - 1)      // last parameter is the self object
        });
        
        auto addressOfField = selfExpr->identifier->symbol->addressOfField;
        writer.write({
            .opcode = OP_PUTFIELD,
            .value = addressOfField
        });
    } else if( node->left->kind == memberAccessExpr) {
        auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(node->left);
        auto identifierExpr = std::static_pointer_cast<IdentifierExpr>(memberAccessExpr->parent);
        writer.write({
            .opcode = OP_ISTORE,
            .value = identifierExpr->symbol->addressOfVariable
        });
        
//        switch (memberAccessExpr->member->kind) {
//            case :
//                <#statements#>
//                break;
//                
//            default:
//                break;
//        }
    } else {
        assert(false);
    }
    
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
            .value = static_cast<JrInt>(elseInstructions.size())
        });
    }
    
    emit(node->condition);
    writer.write({
        .opcode = OP_IFLE,
        .value = static_cast<JrInt>(instructions.size())
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
    assert(node->symbol->flag == funcSymbol);
    auto function = Global::functions[node->symbol->addressOfFunc];
    
    context->entry(function);
    IRGen generator(context);
    generator.emit(node->codeBlock);
    auto instructions = generator.getInstructions();
    
    assert(function != nullptr && function->instructions.size() == 0);
    function->instructions = instructions;
    context->leave(function);
}

void IRGen::emit(ReturnStatement::Pointer node) {
    Opcode op = OP_RETURN;
    if(node->expr != nullptr) {
        emit(node->expr);
        op = OP_IRETURN;
    }
    
    writer.write({
        .opcode = op
    });
}

void IRGen::emit(ArrayLiteralExpr::Pointer node) {
    for(auto item: node->items) {
        emit(item);
    }
    
    writer.write({
        .opcode = OP_ICONST,
        .value = static_cast<JrInt>(node->items.size())
    });
    
    writer.write({
        .opcode = OP_ONEWARRAY,
        .value = JrType::Any->addressOfType
    });
}

void IRGen::emit(ClassDecl::Pointer node) {
    for(auto member: node->members) {
        if(member->kind == funcDecl || member->kind == constructorDecl) {
            emit(member);
        }
    }
}

void IRGen::emit(ConstructorDecl::Pointer node) {
    
    assert(node->symbol->flag == constructorSymbol);
    auto function = Global::functions[node->symbol->addressOfFunc];
    
    context->entry(function);
    IRGen generator(context);
    generator.emit(node->codeBlock);
    auto instructions = generator.getInstructions();
    
    assert(function != nullptr && function->instructions.size() == 0);
    function->instructions = instructions;
    
    context->leave(function);
    
}

void IRGen::emit(MemberAccessExpr::Pointer node) {
    emit(node->parent);
    emit(node->member);
}

void IRGen::emit(SubscriptExpr::Pointer node) {
    
}
