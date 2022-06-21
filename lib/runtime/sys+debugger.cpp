#include "joyeer/runtime/sys.h"
#include "joyeer/runtime/arguments.h"
#include "joyeer/runtime/executor.h"
#include "joyeer/vm/frame.h"


Value Global_$_debugPrintCurrentStackFrames(Executor* executor, Arguments* arguments) {

    auto totalFrameCount = executor->getStackFrameCount();
    printf("stack-frames-count: %ld\n", totalFrameCount);

    for(auto i = totalFrameCount; i > 0; i -- ) {
        auto index = i - 1 ;
        printf("--------------------------\n");
        printf("stack: %ld\n", index);
        auto frameSnapshot = executor->getStackFrameSnapshot(index);
        auto frameType = StackFrame::getFrameType(frameSnapshot.fp + executor->stack);
        switch (frameType) {
            case StackFrame::Type::MODULE:
                printf("type: MODULE\n");
                break;
            case StackFrame::Type::FUNC_CALL:
                printf("type: FUNC_CALL\n");
                break;
            default:
                assert(false);
        }

    }

    return 0;
}