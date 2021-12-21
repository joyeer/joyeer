#include "joyeer/compiler/IRGen.h"
#include "joyeer/common/diagnostic.h"
#include "joyeer/compiler/compiler+service.h"

////////////////////////////////////////////////////////
// IRGen
////////////////////////////////////////////////////////

IRGen::IRGen(CompileContext::Ptr context):
context(std::move(context)) {
}

void IRGen::emit(const Node::Ptr& node) {

#define NODE_EMIT(flag, type) \
    case flag: \
        emit(std::static_pointer_cast< type >(node)); \
        break;

    switch (node->kind) {
        NODE_EMIT(SyntaxKind::module, ModuleDecl)
        NODE_EMIT(SyntaxKind::varDecl, VarDecl)
        NODE_EMIT(SyntaxKind::funcDecl, FuncDecl)
        NODE_EMIT(SyntaxKind::memberFuncCallExpr, MemberFuncCallExpr)
        NODE_EMIT(SyntaxKind::classDecl, ClassDecl)
        NODE_EMIT(SyntaxKind::stmtsBlock, StmtsBlock)
        NODE_EMIT(SyntaxKind::ifStmt, IfStmt)
        NODE_EMIT(SyntaxKind::expr, Expr)
        NODE_EMIT(SyntaxKind::prefixExpr, PrefixExpr)
        NODE_EMIT(SyntaxKind::identifierExpr, IdentifierExpr)
        NODE_EMIT(SyntaxKind::parenthesizedExpr, ParenthesizedExpr)
        NODE_EMIT(SyntaxKind::arguCallExpr, ArguCallExpr)
        NODE_EMIT(SyntaxKind::funcCallExpr, FuncCallExpr)
        NODE_EMIT(SyntaxKind::memberAccessExpr, MemberAccessExpr)
        NODE_EMIT(SyntaxKind::literalExpr, LiteralExpr)
        NODE_EMIT(SyntaxKind::arrayLiteralExpr, ArrayLiteralExpr)
        NODE_EMIT(SyntaxKind::dictLiteralExpr, DictLiteralExpr)
        NODE_EMIT(SyntaxKind::assignExpr, AssignExpr)
        NODE_EMIT(SyntaxKind::operatorExpr, OperatorExpr)
        NODE_EMIT(SyntaxKind::returnStmt, ReturnStmt)
        NODE_EMIT(SyntaxKind::subscriptExpr, SubscriptExpr)
        NODE_EMIT(SyntaxKind::whileStmt, WhileStmt)
        NODE_EMIT(SyntaxKind::importStmt, ImportStmt)
        default:
            assert(false);
    }
}

ModuleType::Ptr IRGen::emit(const ModuleDecl::Ptr& decl) {

    auto moduleType = std::static_pointer_cast<ModuleType>(decl->type);
    context->visit(CompileStage::visitModule, decl, [this, decl]() {
        for(const auto& member: decl->statements) {
            emit(member);
        }
    });

    writer.write({
        .opcode = OP_RETURN,
        .value = 0
    });
    moduleType->instructions = writer.instructions;

    return moduleType;
}

void IRGen::emit(const FuncCallExpr::Ptr& funcCallExpr) {

    context->visit(CompileStage::visitFuncCall, funcCallExpr, [this, funcCallExpr](){
        
        for(const auto& argument : funcCallExpr->arguments) {
            emit(argument);
        }

        auto funcType = std::static_pointer_cast<FuncType>(funcCallExpr->type);
        if(funcCallExpr->identifier->kind == SyntaxKind::memberAccessExpr) {
            emit(funcCallExpr->identifier);
        }

        writer.write({
            .opcode = OP_INVOKE,
            .value = funcType->address
        });

    });
}

void IRGen::emit(const MemberFuncCallExpr::Ptr& memberFuncCallExpr) {
    context->visit(CompileStage::visitFuncCall, memberFuncCallExpr, [this, memberFuncCallExpr](){
        for(const auto& argument : memberFuncCallExpr->arguments) {
            emit(argument);
        }
        
        emit(memberFuncCallExpr->callee);
        
        auto funcDef = memberFuncCallExpr->type;
        assert(funcDef != nullptr);
        writer.write({
            .opcode = OP_INVOKE,
            .value = funcDef->address
        });
    });
}

void IRGen::emit(const ArguCallExpr::Ptr& node) {
    emit(node->expr);
}

void IRGen::emit(const LiteralExpr::Ptr& node) {
    auto compiler = context->compiler;

    switch(node->literal->kind) {
        case decimalLiteral:
            writer.write({
                .opcode = OP_ICONST,
                .value = node->literal->intValue
            });
            break;
        case stringLiteral: {
            auto stringSlot = compiler->registerStringResource(node->literal->rawValue);
            writer.write({
                .opcode = OP_SCONST,
                .value = stringSlot
            });
        }
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
    }
}

void IRGen::emit(const VarDecl::Ptr& node) {
    emit(node->initializer);

    auto symbol = context->lookup(node->getSimpleName());

    switch (symbol->flag) {
        case SymbolFlag::var:
            writer.write({
                .opcode = OP_ISTORE,
                .value = symbol->locationInParent
            });
            break;
        case SymbolFlag::field: {
            if(symbol->isStatic) {
                writer.write({
                    .opcode = OP_PUTSTATIC,
                    .pair = {
                            .val1 = symbol->parentTypeSlot,
                            .val2 = symbol->locationInParent
                    }});
                return;
            } else {
                writer.write({
                    .opcode = OP_OLOAD
                 });
            }


        }
            break;
        default:
            assert(false);
    }

}

void IRGen::emit(const PrefixExpr::Ptr& node) {
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

void IRGen::emit(const IdentifierExpr::Ptr& node) {
    auto symbol = context->lookup(node->getSimpleName());
    if(symbol == nullptr) {
        Diagnostics::reportError("[Error][GenCode]");
        return;
    }


    if(symbol->flag  == SymbolFlag::var) {
        auto typeOfVariable = context->compiler->getType(symbol->typeSlot);
        switch (typeOfVariable->kind) {
            case ValueType::Int: {
                writer.write({
                    .opcode = OP_ILOAD,
                    .value = symbol->locationInParent
                });
            }
                break;
            default:
                assert(false);
        }

//        } else if(kind->kind == TypeKind::Class || kind->kind == TypeKind::Any ) {
//            writer.write({
//                .opcode = OP_OLOAD,
//                .value = symbol->addressOfVariable
//            });
//        } else if(kind->kind == TypeKind::Nil) {
//            writer.write({
//                .opcode = OP_OCONST_NIL,
//            });
//        } else {
//            assert(false);
//        }
        return;
    } else if(symbol->flag == SymbolFlag::field) {

        if(symbol->isStatic) {
            // static field
            assert(symbol->parentTypeSlot != -1);
            assert(symbol->locationInParent != -1);
            writer.write({
                                 .opcode = OP_GETSTATIC,
                                 .pair = {
                                         .val1 = symbol->parentTypeSlot,
                                         .val2 = symbol->locationInParent
                                 }
                         });
            return;
        }
    }

    assert(false);
    // Step2: try to find the symbol in symbols
}

void IRGen::emit(const AssignExpr::Ptr& node) {

    if(node->left->kind == SyntaxKind::identifierExpr) {
        emit(node->expr);
        auto identifierExpr = std::static_pointer_cast<IdentifierExpr>(node->left);
        auto symbol = context->lookup(identifierExpr->getSimpleName());

        if(symbol->flag == SymbolFlag::field) {
            assert(symbol->parentTypeSlot != -1);
            assert(symbol->locationInParent != -1);
            if(symbol->isStatic) {
                writer.write({
                             .opcode = OP_PUTSTATIC,
                             .pair = {
                                     .val1 = symbol->parentTypeSlot,
                                     .val2 = symbol->locationInParent
                                     }
                     });
            } else {
                assert(false);
            }
        }
    } else if( node->left->kind == SyntaxKind::selfExpr ) {
        emit(node->expr);
        auto selfExpr = std::static_pointer_cast<SelfExpr>(node->left);
        
        auto function = context->curFuncType();
        
        writer.write({
            .opcode = OP_OLOAD,
            .value = (int32_t)(function->paramTypes.size() - 1)      // last parameter is the self object
        });
        
//        auto addressOfField = selfExpr->identifier->symbol->addressOfField;
//        writer.write({
//            .opcode = OP_PUTFIELD,
//            .value = addressOfField
//        });
        assert(false);
    } else if( node->left->kind == SyntaxKind::memberAccessExpr) {
        emit(node->expr);
        auto memberAccessExpr = std::static_pointer_cast<MemberAccessExpr>(node->left);
        auto identifierExpr = std::static_pointer_cast<IdentifierExpr>(memberAccessExpr->callee);
//        writer.write({
//            .opcode = OP_ISTORE,
//            .value = identifierExpr->symbol->addressOfVariable
//        });
        assert(false);
    } else if(node->left->kind == SyntaxKind::subscriptExpr) {
        
        auto subscriptExpr = std::static_pointer_cast<SubscriptExpr>(node->left);
        emit(subscriptExpr->identifier);
        emit(subscriptExpr->indexExpr);
        emit(node->expr);
        // check identifier's symbol's kind
//        if(subscriptExpr->identifier->symbol->addressOfType == JrObjectArray::Type->addressOfType) {
//            writer.write({
//                .opcode = OP_INVOKE,
//                .value = JrObjectArray_Set::Func->addressOfFunc
//            });
//        } else if(subscriptExpr->identifier->symbol->addressOfType == JrObjectMap::Type->addressOfType) {
//            writer.write({
//                .opcode = OP_INVOKE,
//                .value = JrObjectMap_Insert::Func->addressOfFunc
//            });
//        } else {
//            assert(false);
//        }
        assert(false);
    } else {
        assert(false);
    }
    
}

void IRGen::emit(const Expr::Ptr& node) {
    assert(node->prefix == nullptr);
    assert(node->binaries.empty());
//    if(node->kind == JrObjectString::Type) {
//
//        writer.write({
//            .opcode = OP_NEW,
//            .value = JrObjectStringBuilder::Type->addressOfType
//        });
//
//        // Use the StringBuilder to append the string content
//        for(std::vector<Node::Ptr>::const_reverse_iterator iterator = node->nodes.rbegin(); iterator != node->nodes.rend(); iterator ++ ) {
//            auto n = *iterator;
//            if(n->kind == SyntaxKind::operatorExpr) {
//                auto operatorExpr = std::static_pointer_cast<OperatorExpr>(n);
//                assert(operatorExpr->token->rawValue == "+");
//            } else {
//                writer.write({
//                    .opcode = OP_DUP
//                });
//
//                emit(n);
//
//                writer.write({
//                    .opcode = OP_INVOKE,
//                    .value = JrObjectStringBuilder_Append::Func->addressOfFunc
//                });
//            }
//        }
//
//        writer.write({
//            .opcode = OP_INVOKE,
//            .value = JrObjectStringBuilder_toString::Func->addressOfFunc
//        });
//
//
//    } else {
        for(const auto& n: node->nodes) {
            emit(n);
        }
//    }
}

void IRGen::emit(const OperatorExpr::Ptr& node) {
    
    static std::unordered_map<std::string, Opcode> imaps {
        {
            { Operators::MINUS, OP_ISUB },
            { Operators::PLUS, OP_IADD },
            { Operators::DIV, OP_IDIV },
            { Operators::MULTIPLY, OP_IMUL },
            { Operators::PERCENTAGE, OP_IREM },
            { Operators::GREATER, OP_ICMP_G },
            {Operators::GREATER_EQ, OP_ICMP_GE },
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

void IRGen::emit(const ParenthesizedExpr::Ptr& node) {
    emit(node->expr);
}

void IRGen::emit(const IfStmt::Ptr& node) {
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
            .value = static_cast<int32_t>(elseInstructions.size())
        });
    }
    
    emit(node->condition);

    writer.write({
        .opcode = OP_IFNE,
        .value = static_cast<int32_t>(instructions.size())
    });
    writer.write(instructions);
    writer.write(elseInstructions);
    
}

void IRGen::emit(const WhileStmt::Ptr& node) {
    IRGen gen(context);
    gen.emit(node->codeBlock);
    
    IRGen conditionGen(context);
    conditionGen.emit(node->expr);
    
    auto position = writer.instructions.size();
    writer.write(conditionGen.writer.instructions);
    writer.write({
        .opcode = OP_IFLE,
        .value = static_cast<int32_t>(gen.writer.instructions.size() + 1)
    });
    writer.write(gen.writer.instructions);
    writer.write({
        .opcode = OP_GOTO,
        .value = static_cast<int32_t>(position - writer.instructions.size() - 1)
    });
    
}

void IRGen::emit(const StmtsBlock::Ptr& node) {
    context->visit(CompileStage::visitCodeBlock, node, [this, node]() {
        for(const auto& statement: node->statements) {
            emit(statement);
        }
    });
}

void IRGen::emit(const FuncDecl::Ptr& node) {

    auto function = std::static_pointer_cast<FuncType>(node->type);

    IRGen generator(context);

    context->visit(CompileStage::visitFuncDecl, node, [node, &generator](){
        generator.emit(node->codeBlock);
    });

    auto instructions = generator.writer.instructions;
    
    assert(function != nullptr && function->instructions.empty());
    function->instructions = instructions;
}

void IRGen::emit(const ReturnStmt::Ptr& node) {
    Opcode op = OP_RETURN;
    if(node->expr != nullptr) {
        emit(node->expr);
        op = OP_IRETURN;
    }
    
    writer.write({
        .opcode = op
    });
}

void IRGen::emit(const ArrayLiteralExpr::Ptr& node) {
    for(const auto& item: node->items) {
        emit(item);
    }
    
    writer.write({
        .opcode = OP_ICONST,
        .value = static_cast<int32_t>(node->items.size())
    });
    
//    writer.write({
//        .opcode = OP_ONEWARRAY,
//        .value = Type::Any->addressOfType
//    });

    assert(false);
}

void IRGen::emit(const DictLiteralExpr::Ptr& node) {
//    writer.write({
//        .opcode = OP_NEW,
//        .value = JrObjectMap::Constructor->addressOfFunc
//    });
//
//    for(auto item: node->items) {
//        writer.write({
//           .opcode = OP_DUP
//        });
//
//        emit(std::get<0>(item));
//        emit(std::get<1>(item));
//
//        writer.write({
//            .opcode = OP_INVOKE,
//            .value = JrObjectMap_Insert::Func->addressOfFunc
//        });
//    }
    assert(false);
}

void IRGen::emit(const ClassDecl::Ptr& node) {
    for(const auto& member: node->members->statements) {
        if(member->kind == SyntaxKind::funcDecl) {
            emit(member);
        }
    }
}

void IRGen::emit(const MemberAccessExpr::Ptr& node) {
    emit(node->callee);
    
//    writer.write({
//        .opcode = OP_GETFIELD,
//        .value = node->member->symbol->addressOfField
//    });
    assert(false);
}

void IRGen::emit(const SubscriptExpr::Ptr& node) {
    
    // handle the index expr of array access
    emit(node->indexExpr);
    emit(node->identifier);
//    assert(node->identifier->symbol != nullptr);
    
//    if (node->identifier->symbol->addressOfType == JrObjectMap::Type->addressOfType) {
//        writer.write({
//            .opcode = OP_INVOKE,
//            .value = JrObjectMap_Get::Func->addressOfFunc
//        });
//    } else if(node->identifier->symbol->addressOfType == JrObjectArray::Type->addressOfType) {
//        writer.write({
//            .opcode = OP_INVOKE,
//            .value = JrObjectArray_Get::Func->addressOfFunc
//        });
//    } else {
//        assert(false);
//    }

    assert(false);
}

void IRGen::emit(const ImportStmt::Ptr& node) {
    
}
