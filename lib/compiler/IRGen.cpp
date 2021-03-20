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
        case sourceBlock:
            emit(std::static_pointer_cast<SourceBlock>(node));
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
        case memberFuncCallExpr:
            emit(std::static_pointer_cast<MemberFuncCallExpr>(node));
            break;
        case constructorDecl:
            emit(std::static_pointer_cast<ConstructorDecl>(node));
            break;
        case classDecl:
            emit(std::static_pointer_cast<ClassDecl>(node));
            break;
        case codeBlock:
            emit(std::static_pointer_cast<CodeBlock>(node));
            break;
        case ifStatement:
            emit(std::static_pointer_cast<IfStatement>(node));
            break;
        case expr:
            emit(std::static_pointer_cast<Expr>(node));
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
            emit(std::static_pointer_cast<DictLiteralExpr>(node));
            break;
        case assignmentExpr:
            emit(std::static_pointer_cast<AssignmentExpr>(node));
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
        case whileStatement:
            emit(std::static_pointer_cast<WhileStatement>(node));
            break;
        case fileimportDecl:
            emit(std::static_pointer_cast<FileImportDecl>(node));
            break;
        default:
            assert(false);
    }
}

JrModuleClass* IRGen::emit(SourceBlock::Ptr block) {
    
    assert(block->symbol->flag == moduleSymbol);
    auto moduleType = (JrModuleClass*)Global::types[block->symbol->addressOfType];
    assert(moduleType->constructors.size() == 1);
    auto func = Global::functions[moduleType->constructors.back()];
    
    context->entry(moduleType);
    context->entry(func);
    context->visit(visitSourceBlock, [this, block]() {
        for(auto& statement: block->statements) {
            emit(statement);
        }
    });
    context->leave(func);
    context->leave(moduleType);
    
    func->instructions = writer.instructions;
    return moduleType;
}

void IRGen::emit(FuncCallExpr::Ptr funcCallExpr) {
    context->visit(visitFuncCall, [this, funcCallExpr](){
        
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
            if(funcCallExpr->identifier->kind == memberAccessExpr) {
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
    context->visit(visitFuncCall, [this, memberFuncCallExpr](){
        for(auto argument : memberFuncCallExpr->arguments) {
            emit(argument);
        }
        
        emit(memberFuncCallExpr->parent);
        
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

void IRGen::emit(LetDecl::Ptr node) {
    emit(node->initializer);
    
    // TODO: detect the variable's type
    writer.write({
        .opcode = OP_ISTORE,
        .value = node->symbol->addressOfVariable
    });
    
}

void IRGen::emit(VarDecl::Ptr node) {
    emit(node->initializer);
    
    auto function = context->curFunction();
    switch (node->symbol->flag) {
        case varSymbol:
            writer.write({
                .opcode = OP_ISTORE,
                .value = node->symbol->addressOfVariable
            });
            break;
        case fieldSymbol:
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
        Diagnostics::reportError(L"[Error][GenCode]");
        return;
    }
    
    if(symbol->flag  == varSymbol) {
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
    } else if(symbol->flag == fieldSymbol) {
        
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
    
    
    if(node->left->kind == identifierExpr) {
        emit(node->expr);
        auto identifierExpr = std::static_pointer_cast<IdentifierExpr>(node->left);
        if(identifierExpr->symbol->flag == fieldSymbol) {
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
        
    } else if( node->left->kind == selfExpr ) {
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
    } else if( node->left->kind == memberAccessExpr) {
        emit(node->expr);
        auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(node->left);
        auto identifierExpr = std::static_pointer_cast<IdentifierExpr>(memberAccessExpr->parent);
        writer.write({
            .opcode = OP_ISTORE,
            .value = identifierExpr->symbol->addressOfVariable
        });
    } else if(node->left->kind == subscriptExpr) {
        
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
            if(n->kind == operatorExpr) {
                auto operatorExpr = std::static_pointer_cast<OperatorExpr>(n);
                assert(operatorExpr->token->rawValue == L"+");
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
    
    static std::unordered_map<std::wstring, Opcode> imaps {
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

void IRGen::emit(IfStatement::Ptr node) {
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

void IRGen::emit(WhileStatement::Ptr node) {
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

void IRGen::emit(CodeBlock::Ptr node) {
    context->visit(visitCodeBlock, [this, node]() {
        for(auto statement: node->statements) {
            emit(statement);
        }
    });
}

void IRGen::emit(FuncDecl::Ptr node) {
    assert(node->symbol->flag == funcSymbol);
    auto function = Global::functions[node->symbol->addressOfFunc];
    
    context->entry(function);
    IRGen generator(context);
    generator.emit(node->codeBlock);
    auto instructions = generator.writer.instructions;
    
    assert(function != nullptr && function->instructions.size() == 0);
    function->instructions = instructions;
    context->leave(function);
}

void IRGen::emit(ReturnStatement::Ptr node) {
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
    for(auto member: node->members) {
        if(member->kind == funcDecl || member->kind == constructorDecl) {
            emit(member);
        }
    }
}

void IRGen::emit(ConstructorDecl::Ptr node) {
    
    assert(node->symbol->flag == constructorSymbol);
    auto function = Global::functions[node->symbol->addressOfFunc];
    
    context->entry(function);
    IRGen generator(context);
    generator.emit(node->codeBlock);
    auto instructions = generator.writer.instructions;
    if (instructions.back().opcode == OP_RETURN) {
        instructions.pop_back();
        instructions.push_back(Instruction{
            .opcode = OP_ORETURN,
            .value = function->addressOfOwnerType
        });
    }
    assert(function != nullptr && function->instructions.size() == 0);
    function->instructions = instructions;
    
    context->leave(function);
}

void IRGen::emit(MemberAccessExpr::Ptr node) {
    emit(node->parent);
    
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

void IRGen::emit(FileImportDecl::Ptr node) {
    
}
