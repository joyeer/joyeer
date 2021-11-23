//
// Created by Qing Xu on 2021/11/6.
//

#ifndef __joyeer_vm_frames_h__
#define __joyeer_vm_frames_h__

#include "joyeer/vm/types.h"

struct ModuleEntryFrame;


// define the frame types
#define DECLARE_TYPE(type, ignore) type,
#define STACK_FRAME_TYPE_LIST(D) \
    D(FILE_MODULE, FileModuleEntryFrame) \
    D(FUNC_CALL, FuncCallFrame)

// Base class for all Frame
struct StackFrame {

    enum Type {
        NO_FRAME_TYPE = -1,
        STACK_FRAME_TYPE_LIST(DECLARE_TYPE)
        NUMBER_OF_FRAME_TYPES
    };

    [[nodiscard]] virtual Type type() const = 0;
};

#undef DECLARE_TYPE
#undef STACK_FRAME_TYPE_LIST

/**
 *  slot  +------------------+
 *  -n-1  |  param n         |
 *        |------------------+
 *  -n    |  param n-1       |
 *  ....  |  ....            |
 *  -2    |  param 1         |
 *        |------------------+
 *  -1    |  param 0         |
 *        |------------------+
 *  0     + return addr      |
 *        |------------------+
 *  1     | frame ptr        |  -> frame ptr
 *        +------------------+
 *
 */

struct FuncCallFrame : public StackFrame {

    [[nodiscard]] Type type() const override {
        return StackFrame::Type::FUNC_CALL;
    }
};

/**
 * slot +---------------------+
 *  0   | module address      |
 * -----+---------------------+
 *  1   | frame ptr           |
 * -----+---------------------+
 */

struct ModuleEntryFrame : public StackFrame {

    [[nodiscard]] Type type() const override {
        return StackFrame::Type::FILE_MODULE;
    }
};

struct Stack {
public:
    struct StackSlot {
    };
};



#endif //__joyeer_vm_frames_h__
