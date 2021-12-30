//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/interpreter.h"
#include "joyeer/compiler/opcode.h"

struct Interpreter;

Arguments::Arguments(Executor *executor):
executor(executor) {
}

Value Arguments::getArgument(Slot slot) {
    auto pValue = (Value*)(executor->stack + executor->fp - kValueSize - slot * kValueSize);
    return *pValue;
}

#define HANDLE_BYTECODE(OP) \
        case OP_##OP: \
            Handle_##OP(bytecode); \
            goto next;

#define HANDLE_BYTECODE_RETURN(OP) \
        case OP_##OP: \
            Handle_##OP(bytecode); \
            goto exit;

struct Interpreter {
    intptr_t cp = 0;
    Executor* executor;
    IsolateVM* isolateVm;
    Bytecodes* bytecodes;

    Interpreter(Executor* executor, Bytecodes* bytecodes) :
            executor(executor),
            bytecodes(bytecodes) {
        isolateVm = executor->isolateVM;
    }

    [[maybe_unused]] void run() {
loop:
        auto bytecode = *(Int *)(bytecodes->bytecodes + cp);

        auto opcode = OP_FROM_BYTECODE(bytecode);

        switch (opcode) {
            HANDLE_BYTECODE(NOP)
            HANDLE_BYTECODE(OCONST_NIL)
            HANDLE_BYTECODE(ICONST)
            HANDLE_BYTECODE(SCONST)
            HANDLE_BYTECODE(ISTORE)
            HANDLE_BYTECODE(OLOAD)
            HANDLE_BYTECODE(ILOAD)
            HANDLE_BYTECODE(NEW)
            HANDLE_BYTECODE(PUTFIELD)
            HANDLE_BYTECODE(GETFIELD)
            HANDLE_BYTECODE(PUTSTATIC)
            HANDLE_BYTECODE(GETSTATIC)
            HANDLE_BYTECODE(IAND)
            HANDLE_BYTECODE(ICMP_G)
            HANDLE_BYTECODE(ICMP_GE)
            HANDLE_BYTECODE(ICMP_L)
            HANDLE_BYTECODE(ICMP_LE)
            HANDLE_BYTECODE(ICMP_NE)
            HANDLE_BYTECODE(ICMP_EQ)
            HANDLE_BYTECODE(IADD)
            HANDLE_BYTECODE(ISUB)
            HANDLE_BYTECODE(IMUL)
            HANDLE_BYTECODE(IDIV)
            HANDLE_BYTECODE(IREM)
            HANDLE_BYTECODE(INEG)
            HANDLE_BYTECODE(IFEQ)
            HANDLE_BYTECODE(IFNE)
            HANDLE_BYTECODE(IFLT)
            HANDLE_BYTECODE(IFLE)
            HANDLE_BYTECODE(IFGT)
            HANDLE_BYTECODE(IFGE)
            HANDLE_BYTECODE_RETURN(RETURN)
            HANDLE_BYTECODE_RETURN(IRETURN)
            HANDLE_BYTECODE_RETURN(ORETURN)
            HANDLE_BYTECODE(ONEWARRAY)
            HANDLE_BYTECODE(INVOKE)
            HANDLE_BYTECODE(DUP)
            HANDLE_BYTECODE(GOTO)
            HANDLE_BYTECODE(DEBUG)
            default:
                assert(false);

        }
    next:
        cp += kIntSize;
        goto loop;

    exit:
        return;
    }

    inline void push(Value value) const {
        executor->push(value);
    }

    inline Value pop() {
        return executor->pop();
    }

    inline void Handle_NOP(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_NOP);
        assert(false);
    }

    inline void Handle_OCONST_NIL(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_OCONST_NIL);
        assert(false);
    }

    inline void Handle_ICONST(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ICONST);
        auto value = VALUE_FROM_BYTECODE(bytecode);
        push({ .intValue = value });
    }

    inline void Handle_SCONST(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_SCONST);
        auto value = VALUE_FROM_BYTECODE(bytecode);

        auto stringValue = executor->isolateVM->strings[value];
        assert(false);
    }

    inline void Handle_ISTORE(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ISTORE);
        auto slot = VALUE_FROM_BYTECODE(bytecode);
        auto newValue = pop();

        FuncCallFrame::setLocalVar(executor->getCurrentFrame(), slot, newValue);
    }

    inline void Handle_OLOAD(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_OLOAD);
        assert(false);
    }

    inline void Handle_ILOAD(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ILOAD);
        auto value = VALUE_FROM_BYTECODE(bytecode);

        auto intValue = FuncCallFrame::getLocalVar(executor->getCurrentFrame(), value);
        push(intValue);
    }

    inline void Handle_NEW(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_NEW);
        assert(false);
    }

    inline void Handle_PUTFIELD(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_PUTFIELD);
        assert(false);
    }

    inline void Handle_GETFIELD(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_GETFIELD);
        assert(false);
    }

    inline void Handle_PUTSTATIC(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_PUTSTATIC);

        auto classSlotId = VAL1_FROM_BYTECODE(bytecode);    // slot id of class
        auto fieldOffset = VAL2_FROM_BYTECODE(bytecode); // field variable position
        auto value = pop();

        auto klass = (*isolateVm->classes)[classSlotId];
        GC::write(klass->staticArea, value, fieldOffset);
    }

    inline void Handle_GETSTATIC(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_GETSTATIC);
        auto classSlotId = VAL1_FROM_BYTECODE(bytecode);    // slot id of class
        auto fieldOffset = VAL2_FROM_BYTECODE(bytecode);

        auto klass = (*isolateVm->classes)[classSlotId];
        auto value = GC::read(klass->staticArea, fieldOffset);

        push(value);
    }

    inline void Handle_IAND(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IAND);
        assert(false);
    }

    inline void Handle_ICMP_G(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_G);
        assert(false);
    }

    inline void Handle_ICMP_GE(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_GE);
        assert(false);
    }

    inline void Handle_ICMP_L(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_L);
        assert(false);
    }

    inline void Handle_ICMP_LE(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_LE);
        assert(false);
    }

    inline void Handle_ICMP_NE(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_NE);
        assert(false);
    }

    inline void Handle_ICMP_EQ(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_EQ);
        assert(false);
    }

    inline void Handle_IADD(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IADD);

        auto value1 = pop();
        auto value2 = pop();

        auto result = value2.intValue + value1.intValue;
        push({ .intValue = result});
    }

    inline void Handle_ISUB(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ISUB);
        auto value1 = pop();
        auto value2 = pop();

        auto result = value2.intValue - value1.intValue;
        push({ .intValue = result});
    }

    inline void Handle_IMUL(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IMUL);
        auto value1 = pop();
        auto value2 = pop();

        auto result = value2.intValue * value1.intValue;
        push({.intValue = result});
    }

    inline void Handle_IDIV(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IDIV);
        auto value1 = pop();
        auto value2 = pop();

        auto result = value2.intValue / value1.intValue;
        push({ .intValue = result} );
    }

    inline void Handle_IREM(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IREM);
        auto value1 = pop();
        auto value2 = pop();

        auto result = value2.intValue % value1.intValue;
        push({ .intValue = result});
    }

    inline void Handle_INEG(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_INEG);

        auto value1 = pop();
        auto value2 = pop();

        auto result = value2.intValue % value1.intValue;
        push({.intValue = result});
        assert(false);
    }

    inline void Handle_IFEQ(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IFEQ);
        assert(false);
    }

    inline void Handle_IFNE(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IFNE);
        auto value = pop();
        auto offset = VALUE_FROM_BYTECODE(bytecode);

        if (value.intValue == 0) {
            cp += offset * kIntSize;
        }
    }

    inline void Handle_IFLT(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IFLT);
        assert(false);
    }

    inline void Handle_IFLE(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IFLE);
        auto value = pop();

        assert(false);
    }

    inline void Handle_IFGT(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IFGT);
        assert(false);
    }

    inline void Handle_IFGE(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IFGE);
        assert(false);
    }

    inline void Handle_RETURN(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_RETURN);
    }

    inline void Handle_IRETURN(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_IRETURN);
        auto value = pop();

        FuncCallFrame::setReturnValue(executor->getCurrentFrame(), value);
    }

    inline void Handle_ORETURN(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ORETURN);
        assert(false);
    }

    inline void Handle_ONEWARRAY(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_ONEWARRAY);
        auto arrayClass = isolateVm->arrayClass;
        auto value = pop();
        auto arrayObj = arrayClass->allocate(isolateVm, value.intValue);
        for(int i = 0 ; i < value.intValue; i ++ ) {
            auto popValue = pop();
            arrayClass->set(arrayObj, {.intValue = value.intValue - i - 1}, popValue);
        }
        arrayClass->setLength(arrayObj, value);
        push({.intValue = arrayObj});
    }

    inline void Handle_INVOKE(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_INVOKE);
        auto methodSlot = VALUE_FROM_BYTECODE(bytecode);
        auto method = (*isolateVm->methods)[methodSlot];
        switch(method->kind) {
            case MethodKind::C_Method: {
                auto cMethod = dynamic_cast<const CMethod*>(method);
                executor->execute(cMethod);
            }
                break;
            case MethodKind::VM_Method: {
                auto vMethod = dynamic_cast<const VMethod*>(method);
                executor->execute(vMethod);
            }
                break;
        }
    }

    inline void Handle_DUP(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_DUP);
        assert(false);
    }

    inline void Handle_GOTO(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_GOTO);

        auto offset = VALUE_FROM_BYTECODE(bytecode);
        cp += offset * kIntSize;
    }

    inline void Handle_DEBUG(Bytecode bytecode) {
        assert(OP_FROM_BYTECODE(bytecode) == OP_DEBUG);
        assert(false);
    }
};


Executor::Executor(IsolateVM *isolateVM):
isolateVM(isolateVM) {
}

void Executor::execute(const ModuleClass *module) {
    auto savedFP = sp;
    auto frame = &stack[sp];
    ModuleEntryFrame::set(frame, {.intValue = 0}, module->slot);

    push(savedFP, ModuleEntryFrame::size());
    auto method = (*isolateVM->methods)[module->initializerSlot];
    execute(method);
    pop(savedFP);
}

void Executor::execute(const Method *method) {
    auto savedFP = sp;

    push(savedFP, FuncCallFrame::size());

    switch (method->kind) {
        case MethodKind::C_Method:
            invoke(dynamic_cast<const CMethod*>(method));
            break;
        case MethodKind::VM_Method:
            invoke(dynamic_cast<const VMethod*>(method));
            break;
        default:
            assert(false);
    }
    pop(savedFP);

    sp += kValueSize;
}

FramePtr Executor::getCurrentFrame() const {
    return (FramePtr )(stack + fp);
}

void Executor::invoke(const VMethod *method) {
    // prepare the variable
    Arguments arguments(this);

    for(int i = 0; i < method->localVarCount; i ++ ) {
        if(i < method->paramCount) {
            auto argument = arguments.getArgument(method->paramCount - i - 1);
            push(argument);
        } else {
            push({.intValue = 0});
        }
    }
    Interpreter interpreter(this, method->bytecodes);
    interpreter.run();

}

void  Executor::invoke(const CMethod *method) {
    Arguments arguments(this);
    auto value = (*method)(this, &arguments);

    FuncCallFrame::setReturnValue(getCurrentFrame(), value);
}

void Executor::push(Slot frame, int size) {
    frames.push_back(frame);
    sp += size;
    fp = frame;
}

void Executor::pop(Slot frame) {
    assert(frames.back() == frame);
    frames.pop_back();
    sp = frame;
    fp = frames.back();
}

void Executor::push(Value value) {
    *(Int *)(stack + sp) = value.intValue;
    sp += kValueSize;
}

Value Executor::pop() {
    sp -= kValueSize;
    auto result = *(Int *)(stack + sp);

    return {.intValue = result };
}

