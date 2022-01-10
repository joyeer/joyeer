#include "joyeer/compiler/IRGen.h"
#include "joyeer/runtime/diagnostic.h"
#include "joyeer/compiler/compiler+service.h"

////////////////////////////////////////////////////////
// IRGen
////////////////////////////////////////////////////////

IRGen::IRGen(CompileContext::Ptr context):
context(std::move(context)) {
    compiler = this->context->compiler;
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

ModuleClass* IRGen::emit(const ModuleDecl::Ptr& decl) {

    auto module = (ModuleClass*)(compiler->getType(decl->typeSlot));
    context->visit(CompileStage::visitModule, decl, [this, decl]() {
        for(const auto& member: decl->statements) {
            emit(member);
        }
    });

    writer.write(Bytecode(OP_RETURN,0));

    // wrap module initializer code into a V Function
    auto moduleInitializer = new Function(decl->getSimpleName(), true);
    moduleInitializer->funcKind = FuncTypeKind::VM_Func;
    moduleInitializer->bytecodes = writer.getBytecodes();
    compiler->declare(moduleInitializer);

    module->staticInitializerSlot = moduleInitializer->slot;
    return module;
}

void IRGen::emit(const FuncCallExpr::Ptr& funcCallExpr) {

    context->visit(CompileStage::visitFuncCall, funcCallExpr, [this, funcCallExpr](){
        
        for(const auto& argument : funcCallExpr->arguments) {
            emit(argument);
        }

        assert(funcCallExpr->funcTypeSlot != -1);
        auto funcType = (Function*)(compiler->getType(funcCallExpr->funcTypeSlot));
        assert(funcType != nullptr);
        if(funcCallExpr->identifier->kind == SyntaxKind::memberAccessExpr) {
            emit(funcCallExpr->identifier);
        }

        writer.write(Bytecode(OP_INVOKE, funcType->slot));

    });
}

void IRGen::emit(const MemberFuncCallExpr::Ptr& memberFuncCallExpr) {
    emit(memberFuncCallExpr->callee);
    context->visit(CompileStage::visitMemberFuncCall, memberFuncCallExpr, [this, memberFuncCallExpr](){
        emit(memberFuncCallExpr->member);
    });
}

void IRGen::emit(const ArguCallExpr::Ptr& node) {
    emit(node->expr);
}

void IRGen::emit(const LiteralExpr::Ptr& node) {
    auto compiler = context->compiler;

    switch(node->literal->kind) {
        case decimalLiteral:
            writer.write(Bytecode(OP_ICONST, node->literal->intValue));
            break;
        case stringLiteral: {
            auto stringSlot = compiler->strings->import(node->literal->rawValue);
            writer.write(Bytecode(OP_SCONST, stringSlot));
        }
            break;
        case booleanLiteral:
            writer.write(Bytecode(OP_ICONST, node->literal->rawValue == Literals::TRUE ? 1 : 0));
            break;
        case nilLiteral:
            writer.write(Bytecode(OP_OCONST_NIL, -1)
            );
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
            writer.write(Bytecode(OP_ISTORE, symbol->locationInParent));
            break;
        case SymbolFlag::field: {
            if(symbol->isStatic) {
                writer.write(Bytecode(OP_PUTSTATIC, symbol->parentTypeSlot, symbol->locationInParent));
            } else {
                writer.write(Bytecode(OP_OLOAD, -1));
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
        writer.write(Bytecode(OP_INEG, -1));
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
                writer.write(Bytecode(OP_ILOAD, symbol->locationInParent));
            }
                break;
            case ValueType::Any:
            case ValueType::Class: {
                writer.write(Bytecode(OP_OLOAD, symbol->locationInParent));
            }
                break;
            default:
                assert(false);
        }

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
            writer.write(Bytecode(OP_GETSTATIC, symbol->parentTypeSlot, symbol->locationInParent));
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
                writer.write(Bytecode(OP_PUTSTATIC, symbol->parentTypeSlot, symbol->locationInParent));
            } else {
                assert(false);
            }
        } else {
            writer.write(Bytecode(OP_ISTORE, symbol->locationInParent));
        }
    } else if( node->left->kind == SyntaxKind::selfExpr ) {
        emit(node->expr);
        auto selfExpr = std::static_pointer_cast<SelfExpr>(node->left);
        auto function = context->curFuncType();
        
        writer.write(Bytecode(OP_OLOAD, (int32_t)(function->paramCount - 1)));      // last parameter is the self object
        
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
        emit(node->expr);
        emit(subscriptExpr->indexExpr);
        emit(subscriptExpr->identifier);

        // check identifier's symbol's kind
        if(subscriptExpr->identifier->typeSlot == compiler->getType(BuildIns::Object_Array)->slot ) {
            writer.write(Bytecode(OP_INVOKE , compiler->getType(BuildIns::Object_Array_Func_set)->slot));
        } else if(subscriptExpr->identifier->typeSlot == compiler->getType(BuildIns::Object_Dict)->slot) {
            writer.write(Bytecode(OP_INVOKE, compiler->getType(BuildIns::Object_Dict_Func_insert)->slot));
        } else {
            assert(false);
        }
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
    
    writer.write(Bytecode(imaps[node->token->rawValue], -1));
}

void IRGen::emit(const ParenthesizedExpr::Ptr& node) {
    emit(node->expr);
}

void IRGen::emit(const IfStmt::Ptr& node) {
    IRGen gen(context);
    gen.emit(node->ifCodeBlock);
    Bytecodes* elseBytecodes = nullptr;

    if(node->elseCodeBlock != nullptr) {
        IRGen elseBlockGenerator(context);
        elseBlockGenerator.emit(node->elseCodeBlock);
        elseBytecodes = elseBlockGenerator.writer.getBytecodes();
                // insert an goto instruction in ifInstructions;
        gen.writer.write(Bytecode(OP_GOTO, elseBlockGenerator.writer.size() ));
    }
    
    emit(node->condition);

    writer.write(Bytecode(OP_IFNE, gen.writer.size()));
    writer.write(gen.writer.getBytecodes());
    if(elseBytecodes) {
        writer.write(elseBytecodes);
    }
    
}

void IRGen::emit(const WhileStmt::Ptr& node) {
    IRGen gen(context);
    gen.emit(node->codeBlock);
    
    IRGen conditionGen(context);
    conditionGen.emit(node->expr);
    
    auto position = writer.size();
    writer.write(conditionGen.writer.getBytecodes());
    writer.write(Bytecode(OP_IFLE, gen.writer.size() + 1));
    writer.write(gen.writer.getBytecodes());
    writer.write(Bytecode(OP_GOTO, position - writer.size() - 1));
}

void IRGen::emit(const StmtsBlock::Ptr& node) {
    context->visit(CompileStage::visitCodeBlock, node, [this, node]() {
        for(const auto& statement: node->statements) {
            emit(statement);
        }
    });
}

void IRGen::emit(const FuncDecl::Ptr& node) {

    auto function = (Function*)compiler->getType(node->typeSlot);

    IRGen generator(context);

    context->visit(CompileStage::visitFuncDecl, node, [node, &generator](){
        generator.emit(node->codeBlock);
    });

    auto bytecodes = generator.writer.getBytecodes();
    
    assert(function != nullptr && function->bytecodes  == nullptr);
    function->bytecodes = bytecodes;
}

void IRGen::emit(const ReturnStmt::Ptr& node) {
    Opcode op = OP_RETURN;
    if(node->expr != nullptr) {
        emit(node->expr);
        op = OP_IRETURN;
    }
    
    writer.write(Bytecode(op, -1));
}

void IRGen::emit(const ArrayLiteralExpr::Ptr& node) {
    for(const auto& item: node->items) {
        emit(item);
    }
    
    writer.write(Bytecode(OP_ICONST, node->items.size()));
    
    writer.write(Bytecode(OP_ONEWARRAY, compiler->getType(BuildIns::Object_Array)->slot));
}

void IRGen::emit(const DictLiteralExpr::Ptr& node) {
    writer.write(Bytecode(OP_NEW, compiler->getType(BuildIns::Object_Dict)->slot));

    for(auto item: node->items) {
        writer.write(Bytecode(OP_DUP, -1));

        emit(std::get<0>(item));
        emit(std::get<1>(item));

        writer.write(Bytecode(OP_INVOKE, compiler->getType(BuildIns::Object_Dict_Func_insert)->slot));
    }
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

    auto typeSlot = node->identifier->typeSlot;
    if(typeSlot == context->compiler->getType(BuildIns::Object_Array)->slot) {
        writer.write(Bytecode(OP_INVOKE, compiler->getType(BuildIns::Object_Array_Func_get)->slot));
    } else if(typeSlot == compiler->getType(BuildIns::Object_Dict)->slot){
        writer.write(Bytecode(OP_INVOKE, compiler->getType(BuildIns::Object_Dict_Func_get)->slot));
    } else {
        assert(false);
    }

}

void IRGen::emit(const ImportStmt::Ptr& node) {
    
}
