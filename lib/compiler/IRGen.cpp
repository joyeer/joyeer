#include "joyeer/compiler/IRGen.h"
#include "joyeer/compiler/diagnostic.h"
#include "joyeer/runtime/buildin.h"
#include "joyeer/runtime/sys/array.h"
#include "joyeer/runtime/sys/map.h"
#include "joyeer/runtime/sys/string.h"
#include <cassert>
#include <unordered_map>


IRGen::IRGen(CompileContext::Ptr context):
context(context) {
}

void IRGen::emit(Node::Ptr node) {
    switch (node->kind) {
        case SyntaxKind::filemodule:
            emit(std::static_pointer_cast<FileModuleDecl>(node));
            break;
        case SyntaxKind::varDecl:
            emit(std::static_pointer_cast<VarDecl>(node));
            break;
        case SyntaxKind::funcDecl:
            emit(std::static_pointer_cast<FuncDecl>(node));
            break;
        case SyntaxKind::memberFuncCallExpr:
            emit(std::static_pointer_cast<MemberFuncCallExpr>(node));
            break;
        case SyntaxKind::classDecl:
            emit(std::static_pointer_cast<ClassDecl>(node));
            break;
        case SyntaxKind::stmtsBlock:
            emit(std::static_pointer_cast<StmtsBlock>(node));
            break;
        case SyntaxKind::ifStmt:
            emit(std::static_pointer_cast<IfStmt>(node));
            break;
        case SyntaxKind::expr:
            emit(std::static_pointer_cast<Expr>(node));
            break;
        case SyntaxKind::prefixExpr:
            emit(std::static_pointer_cast<PrefixExpr>(node));
            break;
        case SyntaxKind::identifierExpr:
            emit(std::static_pointer_cast<IdentifierExpr>(node));
            break;
        case SyntaxKind::parenthesizedExpr:
            emit(std::static_pointer_cast<ParenthesizedExpr>(node));
            break;
        case SyntaxKind::arguCallExpr:
            emit(std::static_pointer_cast<ArguCallExpr>(node));
            break;
        case SyntaxKind::funcCallExpr:
            emit(std::static_pointer_cast<FuncCallExpr>(node));
            break;
        case SyntaxKind::memberAccessExpr:
            emit(std::static_pointer_cast<MemberAccessExpr>(node));
            break;
        case SyntaxKind::literalExpr:
            emit(std::static_pointer_cast<LiteralExpr>(node));
            break;
        case SyntaxKind::arrayLiteralExpr:
            emit(std::static_pointer_cast<ArrayLiteralExpr>(node));
            break;
        case SyntaxKind::dictLiteralExpr:
            emit(std::static_pointer_cast<DictLiteralExpr>(node));
            break;
        case SyntaxKind::assignmentExpr:
            emit(std::static_pointer_cast<AssignmentExpr>(node));
            break;
        case SyntaxKind::operatorExpr:
            emit(std::static_pointer_cast<OperatorExpr>(node));
            break;
        case SyntaxKind::returnStmt:
            emit(std::static_pointer_cast<ReturnStmt>(node));
            break;
        case SyntaxKind::subscriptExpr:
            emit(std::static_pointer_cast<SubscriptExpr>(node));
            break;
        case SyntaxKind::whileStmt:
            emit(std::static_pointer_cast<WhileStmt>(node));
            break;
        case SyntaxKind::fileimportStmt:
            emit(std::static_pointer_cast<FileImportStmt>(node));
            break;
        default:
            assert(false);
    }
}

JrModuleClass* IRGen::emit(FileModuleDecl::Ptr decl) {
    
    assert( decl->symbol->flag == SymbolFlag::fileModuleSymbol);
    auto moduleType = (JrModuleClass*)Global::types[ decl->symbol->addressOfType];
    assert(moduleType->constructors.size() == 1);
    auto func = Global::functions[moduleType->constructors.back()];
    
    context->entry(moduleType);
    context->entry(func);
    context->visit(CompileStage::visitFileModule, [this,  decl]() {
        for(auto& statement:  decl->members->statements) {
            emit(statement);
        }
    });
    context->leave(func);
    context->leave(moduleType);
    
    func->instructions = writer.instructions;
    return moduleType;
}

void IRGen::emit(FuncCallExpr::Ptr funcCallExpr) {
    context->visit(CompileStage::visitFuncCall, [this, funcCallExpr](){
        
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
            if(funcCallExpr->identifier->kind == SyntaxKind::memberAccessExpr) {
                emit(funcCallExpr->identifier);
            }
            
            writer.write({
                .opcode = OP_INVOKE,
                .value = (int32_t)function->addressOfFunc
            });
            
        }
    });
}

void IRGen::emit(MemberFuncCallExpr::Ptr memberFuncCallExpr) {
    context->visit(CompileStage::visitFuncCall, [this, memberFuncCallExpr](){
        for(auto argument : memberFuncCallExpr->arguments) {
            emit(argument);
        }
        
        emit(memberFuncCallExpr->callee);
        
        auto function = Global::functions[memberFuncCallExpr->symbol->addressOfFunc];
        assert(function != nullptr);
        writer.write({
            .opcode = OP_INVOKE,
            .value = (int32_t)function->addressOfFunc
        });
    });
}

void IRGen::emit(ArguCallExpr::Ptr node) {
    emit(node->expr);
}

void IRGen::emit(LiteralExpr::Ptr node) {
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
                .value = node->literal->rawValue == Literals::TRUE ? 1 : 0
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

void IRGen::emit(VarDecl::Ptr node) {
    emit(node->initializer);
    
    auto function = context->curFunction();
    switch (node->symbol->flag) {
        case SymbolFlag::varSymbol:
            writer.write({
                .opcode = OP_ISTORE,
                .value = node->symbol->addressOfVariable
            });
            break;
        case SymbolFlag::fieldSymbol:
            writer.write({
                .opcode = OP_OLOAD,
                .value = (int32_t)(function->paramTypes.size() - 1)      // last parameter is the self object
            });

            writer.write({
                .opcode = OP_PUTFIELD,
                .value = node->symbol->addressOfField
            });
            break;
        default:
            assert(false);
    }

}

void IRGen::emit(PrefixExpr::Ptr node) {
    assert(node->expr != nullptr);
    emit(node->expr);
    if(node->op->token->rawValue == Operators::MINUS) {
        writer.write({
            .opcode = OP_INEG
        });
    } else {
        assert(false);
    }
}

void IRGen::emit(IdentifierExpr::Ptr node) {
    auto symbol = node->symbol;
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error][GenCode]");
        return;
    }
    
    if(symbol->flag  == SymbolFlag::varSymbol) {
        auto type = Global::types[symbol->addressOfType];
        
        if(type == JrPrimaryType::Int) {
            writer.write({
                .opcode = OP_ILOAD,
                .value = symbol->addressOfVariable
            });
        } else if(type->kind == typeObject || type->kind == typeAny ){
            writer.write({
                .opcode = OP_OLOAD,
                .value = symbol->addressOfVariable
            });
        } else if(type->kind == typeNil) {
            writer.write({
                .opcode = OP_OCONST_NIL,
            });
        } else {
            assert(false);
        }
        
        return;
    } else if(symbol->flag == SymbolFlag::fieldSymbol) {
        
        auto function = context->curFunction();
        writer.write({
            .opcode = OP_OLOAD,
            .value = (int32_t)(function->paramTypes.size() - 1)
        });
        writer.write({
            .opcode = OP_GETFIELD,
            .value = symbol->addressOfField
        });
        
        return;
    }

    assert(false);
    // Step2: try to find the symbol in symbols
}

void IRGen::emit(AssignmentExpr::Ptr node) {
    
    
    if(node->left->kind == SyntaxKind::identifierExpr) {
        emit(node->expr);
        auto identifierExpr = std::static_pointer_cast<IdentifierExpr>(node->left);
        if(identifierExpr->symbol->flag == SymbolFlag::fieldSymbol) {
            // If the identifier is a field
            auto function = context->curFunction();
            writer.write({
                .opcode = OP_OLOAD,
                .value = (JrInt)(function->paramTypes.size() - 1)      // last parameter is the self object
            });
            writer.write({
                .opcode = OP_PUTFIELD,
                .value = identifierExpr->symbol->addressOfField
            });
        } else {
            writer.write({
                .opcode = OP_ISTORE,
                .value = identifierExpr->symbol->addressOfVariable
            });
        }
        
    } else if( node->left->kind == SyntaxKind::selfExpr ) {
        emit(node->expr);
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
    } else if( node->left->kind == SyntaxKind::memberAccessExpr) {
        emit(node->expr);
        auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(node->left);
        auto identifierExpr = std::static_pointer_cast<IdentifierExpr>(memberAccessExpr->callee);
        writer.write({
            .opcode = OP_ISTORE,
            .value = identifierExpr->symbol->addressOfVariable
        });
    } else if(node->left->kind == SyntaxKind::subscriptExpr) {
        
        auto subscriptExpr = std::static_pointer_cast<SubscriptExpr>(node->left);
        emit(subscriptExpr->identifier);
        emit(subscriptExpr->indexExpr);
        emit(node->expr);
        // check identifier's symbol's type
        if(subscriptExpr->identifier->symbol->addressOfType == JrObjectArray::Type->addressOfType) {
            writer.write({
                .opcode = OP_INVOKE,
                .value = JrObjectArray_Set::Func->addressOfFunc
            });
        } else if(subscriptExpr->identifier->symbol->addressOfType == JrObjectMap::Type->addressOfType) {
            writer.write({
                .opcode = OP_INVOKE,
                .value = JrObjectMap_Insert::Func->addressOfFunc
            });
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }
    
}

void IRGen::emit(Expr::Ptr node) {
    assert(node->prefix == nullptr);
    assert(node->binaries.size() == 0);
    if(node->type == JrObjectString::Type) {
        
        writer.write({
            .opcode = OP_NEW,
            .value = JrObjectStringBuilder::Type->addressOfType
        });
        
        // Use the StringBuilder to append the string content
        for(std::vector<Node::Ptr>::const_reverse_iterator iterator = node->nodes.rbegin(); iterator != node->nodes.rend(); iterator ++ ) {
            auto n = *iterator;
            if(n->kind == SyntaxKind::operatorExpr) {
                auto operatorExpr = std::static_pointer_cast<OperatorExpr>(n);
                assert(operatorExpr->token->rawValue == "+");
            } else {
                writer.write({
                    .opcode = OP_DUP
                });
                
                emit(n);
                
                writer.write({
                    .opcode = OP_INVOKE,
                    .value = JrObjectStringBuilder_Append::Func->addressOfFunc
                });
            }
        }
        
        writer.write({
            .opcode = OP_INVOKE,
            .value = JrObjectStringBuilder_toString::Func->addressOfFunc
        });
        
    } else {
        for(auto n: node->nodes) {
            emit(n);
        }
    }
    
}

void IRGen::emit(OperatorExpr::Ptr node) {
    
    static std::unordered_map<std::string, Opcode> imaps {
        {
            { Operators::MINUS, OP_ISUB },
            { Operators::PLUS, OP_IADD },
            { Operators::DIV, OP_IDIV },
            { Operators::MULTIPLY, OP_IMUL },
            { Operators::PERCENTAGE, OP_IREM },
            { Operators::GREATER, OP_ICMP_G },
            { Operators::GERATER_EQ, OP_ICMP_GE },
            { Operators::LESS, OP_ICMP_L },
            { Operators::LESS_EQ, OP_ICMP_LE },
            { Operators::NOT_EQUALS, OP_ICMP_NE },
            { Operators::EQUAL_EQUAL, OP_ICMP_EQ },
            { Operators::AND_AND, OP_IAND },
        }
    };
    
    assert(imaps.find(node->token->rawValue) != imaps.end());
    
    writer.write({
        .opcode = imaps[node->token->rawValue],
    });
}

void IRGen::emit(ParenthesizedExpr::Ptr node) {
    emit(node->expr);
}

void IRGen::emit(IfStmt::Ptr node) {
    IRGen gen(context);
    gen.emit(node->ifCodeBlock);
    auto instructions = gen.writer.instructions;
    
    std::vector<Instruction> elseInstructions;
    if(node->elseCodeBlock != nullptr) {
        IRGen elseBlockGenerator(context);
        elseBlockGenerator.emit(node->elseCodeBlock);
        elseInstructions = elseBlockGenerator.writer.instructions;
        
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

void IRGen::emit(WhileStmt::Ptr node) {
    IRGen gen(context);
    gen.emit(node->codeBlock);
    
    IRGen conditionGen(context);
    conditionGen.emit(node->expr);
    
    auto position = writer.instructions.size();
    writer.write(conditionGen.writer.instructions);
    writer.write({
        .opcode = OP_IFLE,
        .value = static_cast<JrInt>(gen.writer.instructions.size() + 1)
    });
    writer.write(gen.writer.instructions);
    writer.write({
        .opcode = OP_GOTO,
        .value = static_cast<JrInt>(position - writer.instructions.size() - 1)
    });
    
}

void IRGen::emit(StmtsBlock::Ptr node) {
    context->visit(CompileStage::visitCodeBlock, [this, node]() {
        for(auto statement: node->statements) {
            emit(statement);
        }
    });
}

void IRGen::emit(FuncDecl::Ptr node) {
    assert(node->symbol->flag == SymbolFlag::funcSymbol);
    auto function = Global::functions[node->symbol->addressOfFunc];
    
    context->entry(function);
    IRGen generator(context);
    generator.emit(node->codeBlock);
    auto instructions = generator.writer.instructions;
    
    assert(function != nullptr && function->instructions.size() == 0);
    function->instructions = instructions;
    context->leave(function);
}

void IRGen::emit(ReturnStmt::Ptr node) {
    Opcode op = OP_RETURN;
    if(node->expr != nullptr) {
        emit(node->expr);
        op = OP_IRETURN;
    }
    
    writer.write({
        .opcode = op
    });
}

void IRGen::emit(ArrayLiteralExpr::Ptr node) {
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

void IRGen::emit(DictLiteralExpr::Ptr node) {
    writer.write({
        .opcode = OP_NEW,
        .value = JrObjectMap::Constructor->addressOfFunc
    });
    
    for(auto item: node->items) {
        writer.write({
           .opcode = OP_DUP
        });
        
        emit(std::get<0>(item));
        emit(std::get<1>(item));
        
        writer.write({
            .opcode = OP_INVOKE,
            .value = JrObjectMap_Insert::Func->addressOfFunc
        });
    }
}

void IRGen::emit(ClassDecl::Ptr node) {
    for(auto member: node->members->statements) {
        if(member->kind == SyntaxKind::funcDecl) {
            emit(member);
        }
    }
}

void IRGen::emit(MemberAccessExpr::Ptr node) {
    emit(node->callee);
    
    writer.write({
        .opcode = OP_GETFIELD,
        .value = node->member->symbol->addressOfField
    });
}

void IRGen::emit(SubscriptExpr::Ptr node) {
    
    // handle the index expr of array access
    emit(node->indexExpr);
    emit(node->identifier);
    assert(node->identifier->symbol != nullptr);
    
    if (node->identifier->symbol->addressOfType == JrObjectMap::Type->addressOfType) {
        writer.write({
            .opcode = OP_INVOKE,
            .value = JrObjectMap_Get::Func->addressOfFunc
        });
    } else if(node->identifier->symbol->addressOfType == JrObjectArray::Type->addressOfType) {
        writer.write({
            .opcode = OP_INVOKE,
            .value = JrObjectArray_Get::Func->addressOfFunc
        });
    } else {
        assert(false);
    }
}

void IRGen::emit(FileImportStmt::Ptr node) {
    
}
