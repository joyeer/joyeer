//
// Created by Qing Xu on 2021/11/7.
//

#include "joyeer/vm/frame.h"

int FuncCallFrame::size() {
    return kMethodSlotOffset;
}

int ModuleEntryFrame::size() {
    return kModuleEntryFrameSize;
}
