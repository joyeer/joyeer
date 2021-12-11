//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/interpreter.h"
#include "joyeer/compiler/opcode.h"

struct Interpreter;
typedef void (*OpcodeHandler)(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);

void OpcodeHandler_NOP(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_OCONST_NIL(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ICONST(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_SCONST(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ISTORE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_OLOAD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ILOAD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_NEW(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_PUTFIELD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_GETFIELD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_PUTSTATIC(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_GETSTATIC(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IAND(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ICMP_G(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ICMP_GE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ICMP_L(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ICMP_LE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ICMP_NE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ICMP_EQ(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IADD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ISUB(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IMUL(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IDIV(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IREM(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_INEG(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IFEQ(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IFNE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IFLT(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IFLE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IFGT(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IFGE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_RETURN(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_IRETURN(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ORETURN(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_ONEWARRAY(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_INVOKE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_DUP(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_GOTO(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);
void OpcodeHandler_DEBUG(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode);

static OpcodeHandler OpcodeHandlers[] = {
        &OpcodeHandler_NOP,
        &OpcodeHandler_OCONST_NIL,
        &OpcodeHandler_ICONST,
        &OpcodeHandler_SCONST,
        &OpcodeHandler_ISTORE,
        &OpcodeHandler_OLOAD,
        &OpcodeHandler_ILOAD,
        &OpcodeHandler_NEW,
        &OpcodeHandler_PUTFIELD,
        &OpcodeHandler_GETFIELD,
        &OpcodeHandler_PUTSTATIC,
        &OpcodeHandler_GETSTATIC,
        &OpcodeHandler_IAND,
        &OpcodeHandler_ICMP_G,
        &OpcodeHandler_ICMP_GE,
        &OpcodeHandler_ICMP_L,
        &OpcodeHandler_ICMP_LE,
        &OpcodeHandler_ICMP_NE,
        &OpcodeHandler_ICMP_EQ,
        &OpcodeHandler_IADD,
        &OpcodeHandler_ISUB,
        &OpcodeHandler_IMUL,
        &OpcodeHandler_IDIV,
        &OpcodeHandler_IREM,
        &OpcodeHandler_INEG,
        &OpcodeHandler_IFEQ,
        &OpcodeHandler_IFNE,
        &OpcodeHandler_IFLT,
        &OpcodeHandler_IFLE,
        &OpcodeHandler_IFGT,
        &OpcodeHandler_IFGE,
        &OpcodeHandler_RETURN,
        &OpcodeHandler_IRETURN,
        &OpcodeHandler_ORETURN,
        &OpcodeHandler_ONEWARRAY,
        &OpcodeHandler_INVOKE,
        &OpcodeHandler_DUP,
        &OpcodeHandler_GOTO,
        &OpcodeHandler_DEBUG,
        };


#define OP_FROM_BYTECODE(bytecode) (BytecodeOp)((bytecode) & 0x000000FF)
#define VALUE_FROM_BYTECODE(bytecode) (Int)(((bytecode) & 0xFFFFFF00) >> kBytecodeOpWidth)

struct Interpreter {
    intptr_t cp = 0;
    Executor* executor;
    Bytecodes* bytecodes;

    explicit Interpreter(Executor* executor, Bytecodes* bytecodes) :
            executor(executor),
            bytecodes(bytecodes) {
    }


    [[maybe_unused]] void run() {
        label:
        auto bytecode = *(Int *)(bytecodes->bytecodes + cp);

        auto op = OP_FROM_BYTECODE(bytecode);
        auto handler = OpcodeHandlers[op];

        handler(executor->isolateVM, this, bytecode);

        switch (op) {
            case OP_RETURN:
                break;
            default:
                cp += kIntSize;
                goto label;
        }
    }

    inline void push(Value value) {
        executor->push(value);
    }
};


void OpcodeHandler_NOP(IsolateVM* isolateVM, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_NOP);
    assert(false);
}

void OpcodeHandler_OCONST_NIL(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_OCONST_NIL);
    assert(false);
}

void OpcodeHandler_ICONST(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ICONST);
    auto value = VALUE_FROM_BYTECODE(bytecode);
    interpreter->push({ .intValue = value });
}

void OpcodeHandler_SCONST(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_SCONST);
    auto value = VALUE_FROM_BYTECODE(bytecode);

    auto stringValue = isolateVm->stringTable[value];
    assert(false);
}

void OpcodeHandler_ISTORE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ISTORE);
    assert(false);
}

void OpcodeHandler_OLOAD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_OLOAD);
    assert(false);
}

void OpcodeHandler_ILOAD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ILOAD);
    assert(false);
}

void OpcodeHandler_NEW(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_NEW);
    assert(false);
}

void OpcodeHandler_PUTFIELD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_PUTFIELD);
    assert(false);
}

void OpcodeHandler_GETFIELD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_GETFIELD);
    assert(false);
}

void OpcodeHandler_PUTSTATIC(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_PUTSTATIC);
    assert(false);
}

void OpcodeHandler_GETSTATIC(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_GETSTATIC);
    assert(false);
}

void OpcodeHandler_IAND(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IAND);
    assert(false);
}

void OpcodeHandler_ICMP_G(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_G);
    assert(false);
}

void OpcodeHandler_ICMP_GE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_GE);
    assert(false);
}

void OpcodeHandler_ICMP_L(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_L);
    assert(false);
}

void OpcodeHandler_ICMP_LE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_LE);
    assert(false);
}

void OpcodeHandler_ICMP_NE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_NE);
    assert(false);
}

void OpcodeHandler_ICMP_EQ(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ICMP_EQ);
    assert(false);
}

void OpcodeHandler_IADD(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IADD);
    assert(false);
}

void OpcodeHandler_ISUB(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ISUB);
    assert(false);
}

void OpcodeHandler_IMUL(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IMUL);
    assert(false);
}

void OpcodeHandler_IDIV(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IDIV);
    assert(false);
}

void OpcodeHandler_IREM(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IREM);
    assert(false);
}

void OpcodeHandler_INEG(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_INEG);
    assert(false);
}

void OpcodeHandler_IFEQ(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IFEQ);
    assert(false);
}

void OpcodeHandler_IFNE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IFNE);
    assert(false);
}

void OpcodeHandler_IFLT(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IFLT);
    assert(false);
}

void OpcodeHandler_IFLE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IFLE);
    assert(false);
}

void OpcodeHandler_IFGT(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IFGT);
    assert(false);
}

void OpcodeHandler_IFGE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IFGE);
    assert(false);
}

void OpcodeHandler_RETURN(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_RETURN);
    assert(false);
}

void OpcodeHandler_IRETURN(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_IRETURN);
    assert(false);
}

void OpcodeHandler_ORETURN(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ORETURN);
    assert(false);
}

void OpcodeHandler_ONEWARRAY(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_ONEWARRAY);
    assert(false);
}

void OpcodeHandler_INVOKE(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_INVOKE);
    assert(false);
}

void OpcodeHandler_DUP(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_DUP);
    assert(false);
}

void OpcodeHandler_GOTO(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_GOTO);
    assert(false);
}

void OpcodeHandler_DEBUG(IsolateVM* isolateVm, Interpreter* interpreter, Bytecode bytecode) {
    assert(OP_FROM_BYTECODE(bytecode) == OP_DEBUG);
    assert(false);
}


Executor::Executor(IsolateVM *isolateVM):
isolateVM(isolateVM) {
}

void Executor::execute(const ModuleClass *moduleClass) {
    auto savedFP = sp;
    auto frame = &stack[sp];
    ModuleEntryFrame::set(frame, {.intValue = 0}, moduleClass->slotID);

    push(savedFP, ModuleEntryFrame::size());
    auto method = (*isolateVM->methodTable)[moduleClass->initializerSlot];
    execute(method);
    pop(savedFP);
}

void Executor::execute(const Method *method) {
    auto savedFP = sp;
    auto frame = &stack[sp];

    push(savedFP, FuncCallFrame::size());

    switch (method->kind) {
        case MethodKind::C_Method:
            execute(dynamic_cast<const CMethod*>(method));
            break;
        case MethodKind::VM_Method:
            execute(dynamic_cast<const VMethod*>(method));
            break;
        default:
            assert(false);
    }

    pop(savedFP);
}

void Executor::execute(const VMethod *method) {
    Interpreter interpreter(this, method->bytecodes);
    interpreter.run();
}

void  Executor::execute(const CMethod *method) {
    Arguments arguments;
    method->operator() (isolateVM, &arguments);
}

void Executor::push(Slot frame, int size) {
    frames.push_back(frame);
    sp += size;
}

void Executor::push(Value value) {
    sp += kValueSize;
}

void Executor::pop(Slot frame) {
    assert(frames.back() == frame);
    frames.pop_back();
    sp = frame;
}
