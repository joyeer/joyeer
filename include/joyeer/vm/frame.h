//
// Created by Qing Xu on 2021/11/6.
//

#ifndef __joyeer_vm_frames_h__
#define __joyeer_vm_frames_h__

#include "joyeer/runtime/types.h"

struct ModuleEntryFrame;


// define the frame types
#define DECLARE_TYPE(type, ignore) type,
#define STACK_FRAME_TYPE_LIST(D) \
    D(MODULE, ModuleEntryFrame) \
    D(FUNC_CALL, FuncCallFrame)

// Base class for all Frame
struct StackFrame {
    constexpr static int kReturnValueOffset = 0;
    constexpr static int kFrameTypeOffset = kValueSize;

    enum Type {
        NO_FRAME_TYPE = -1,
        STACK_FRAME_TYPE_LIST(DECLARE_TYPE)
        NUMBER_OF_FRAME_TYPES
    };

    static StackFrame::Type getFrameType(FramePtr framePtr);
};

#undef DECLARE_TYPE
#undef STACK_FRAME_TYPE_LIST

/**
 *  slot  +------------------+
 *  -n-1  |  param n         |
 *  ----- |------------------+
 *  -n    |  param n-1       |
 *  ....  |  ....            |
 *  -2    |  param 1         |
 *  ----- |------------------+
 *  -1    |  param 0         |
 *  ----- |------------------+
 *  0     + return addr      |
 *        |------------------+
 *  1     | frame type       |
 *        +------------------+
 *  2
 */

struct FuncCallFrame : public StackFrame {
    constexpr static int kMethodSlotOffset = kFrameTypeOffset + kValueSize;
    constexpr static int kFuncCallFrameSize = kMethodSlotOffset + kValueSize;

    static int size();

    static void set(FramePtr frame, Value returnValue, Slot methodSlot) {
        *(Value*)(frame + kReturnValueOffset) = returnValue;
        *(Value*)(frame + kFrameTypeOffset) = StackFrame::Type::FUNC_CALL ;
        *(Value*)(frame + kMethodSlotOffset) = methodSlot ;
    }

    static Slot getMethodSlot(FramePtr framePtr);

    // Return the local variable in stack
    static Value getLocalVar(FramePtr frame, Slot slot) {
        return *(Value*)(frame + kFuncCallFrameSize + slot * kValueSize);
    }

    static void setLocalVar(FramePtr frame, Slot slot, Value newValue) {
        *(Value*)(frame + kFuncCallFrameSize + slot * kValueSize) = newValue;
    }

    static void setReturnValue(FramePtr frame, Value newValue) {
        *(Value*)(frame + kReturnValueOffset) = newValue;
    }

    static Value getReturnValue(FramePtr frame) {
        return *(Value*)(frame + kReturnValueOffset) ;
    }
};

/**
 * slot +---------------------+
 *  0   | return value        |
 * -----+---------------------+
 *  1   | frame type          |
 * -----+---------------------+
 *  2   | module slot id      |
 * -----+---------------------+
 */

struct ModuleEntryFrame : public StackFrame {
    constexpr static int kModuleSlotOffset = kFrameTypeOffset + kValueSize;
    constexpr static int kModuleEntryFrameSize = kModuleSlotOffset + kValueSize;

    static int size();

    static void set(FramePtr frame, Value returnValue, Slot moduleSlot) {
        *(Value*)(frame + kReturnValueOffset) = returnValue;
        *(Value*)(frame + kFrameTypeOffset) = StackFrame::Type::MODULE ;
        *(Value*)(frame + kModuleSlotOffset) = moduleSlot ;
    }

    static Slot getModuleSlot(FramePtr framePtr);

    static Value getReturnValue(FramePtr frame) {
        return *(Value*)(frame + kReturnValueOffset);
    }
};


#endif //__joyeer_vm_frames_h__
