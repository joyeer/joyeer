//
// Created by Qing Xu on 2021/11/6.
//

#ifndef __joyeer_vm_frames_h__
#define __joyeer_vm_frames_h__

#include "joyeer/vm/types.h"

// Base class for all Frame
struct StackFrame {

};

/**
 *  slot  +------------------------
 *  -n-1  |  param n
 *        |--------------
 *  -n    |  param n-1
 *  ....  |  ....
 *  -2    |  param 1
 *        |-----------------------
 *  -1    |  param 0
 *        |-----------------------
 *  0     + return value
 */

struct FuncCallFrame : public StackFrame {
    FuncCallFrame(JrAddress address) {

    }
};


struct FileModuleEntryFrame : public StackFrame {

};

struct Stack {
public:
    struct StackSlot {

    };
    
};

#endif //__joyeer_vm_frames_h__
