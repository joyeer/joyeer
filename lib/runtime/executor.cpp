#include "joyeer/runtime/executor.h"
#include <cassert>

size_t Executor::getStackFrameCount() const {
    return frames.size();
}

StackFrameSnapshot Executor::getStackFrameSnapshot(int index) const {
    assert(index < frames.size());

    if(index == frames.size() - 1) {
        // last frame
        return {
            .fp = (AddressPtr)frames[index],
            .sp = this->sp
        };
    } else {
        return {
            .fp = (AddressPtr)frames[index],
            .sp = (AddressPtr)frames[index+ 1]
        };
    }
}