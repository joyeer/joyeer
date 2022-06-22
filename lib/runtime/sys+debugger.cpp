#include "joyeer/runtime/sys.h"
#include "joyeer/runtime/arguments.h"
#include "joyeer/runtime/executor.h"
#include "joyeer/vm/frame.h"


void debugPrintModuleEntryFrame(Executor* executor, FramePtr framePtr) {
    printf("type: MODULE\n");
    auto moduleSlot = ModuleEntryFrame::getModuleSlot(framePtr);
    auto moduleClass = executor->vm->getType(moduleSlot);
    assert(moduleClass->kind == ValueType::Module);
    printf("name: %s\n", moduleClass->name.c_str());
}

void debugPrintFuncCallFrame(Executor* executor, FramePtr framePtr) {
    printf("type: FUNC_CALL\n");
    auto methodSlot = FuncCallFrame::getMethodSlot(framePtr);
    auto methodClass = executor->vm->getType(methodSlot);
    assert(methodClass->kind == ValueType::Func );
    printf("name: %s\n", methodClass->name.c_str());
}

Value Global_$_debugPrintCurrentStackFrames(Executor* executor, Arguments* arguments) {

    auto totalFrameCount = executor->getStackFrameCount();
    printf("stack-frames-count: %ld\n", totalFrameCount);

    for(auto i = totalFrameCount; i > 0; i -- ) {
        auto index = i - 1 ;
        printf("--------------------------\n");
        printf("stack: %ld\n", index);
        auto frameSnapshot = executor->getStackFrameSnapshot(index);
        auto framePtr = frameSnapshot.fp + executor->stack;
        auto frameType = StackFrame::getFrameType(framePtr);
        switch (frameType) {
            case StackFrame::Type::MODULE:
                debugPrintModuleEntryFrame(executor, framePtr);
                break;
            case StackFrame::Type::FUNC_CALL:
                debugPrintFuncCallFrame(executor, framePtr);
                break;
            default:
                assert(false);
        }

    }

    return 0;
}