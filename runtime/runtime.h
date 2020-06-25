#ifndef __joyeer_runtime_runtime_h__
#define __joyeer_runtime_runtime_h__

#include <stdint.h>

// Runtime stack for VM
struct JrRuntimeStack {
    const static uint32_t Size = 1024 * 1024 * 4;
    
    uint32_t pointer;
    uint8_t data[JrRuntimeStack::Size];
};

// The runtime context for VM
struct JrRuntimeContext {
    JrRuntimeStack *stack;
};


#endif
