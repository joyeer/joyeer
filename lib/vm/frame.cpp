//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/frame.h"

StackFrame::Type StackFrame::getFrameType(FramePtr framePtr) {
    return (StackFrame::Type)(*(Value*)(framePtr + kFrameTypeOffset));
}

int FuncCallFrame::size() {
    return kFuncCallFrameSize;
}

Slot FuncCallFrame::getMethodSlot(FramePtr framePtr) {
    return *(Slot*)(framePtr + kMethodSlotOffset);
}

int ModuleEntryFrame::size() {
    return kModuleEntryFrameSize;
}

Slot ModuleEntryFrame::getModuleSlot(FramePtr framePtr) {
    return *(Slot*)(framePtr + kModuleSlotOffset);
}