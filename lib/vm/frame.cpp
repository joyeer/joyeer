//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/frame.h"

StackFrame::Type StackFrame::getFrameType(FramePtr framePtr) {
    return (StackFrame::Type)(*(Value*)(framePtr + kFrameTypeOffset));
}

int FuncCallFrame::size() {
    return kMethodSlotOffset;
}

int ModuleEntryFrame::size() {
    return kModuleEntryFrameSize;
}
