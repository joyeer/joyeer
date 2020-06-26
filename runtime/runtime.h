#ifndef __joyeer_runtime_runtime_h__
#define __joyeer_runtime_runtime_h__

#include <stdint.h>

// Runtime stack for VM
struct JrRuntimeStack {
    const static uint32_t Size = 1024 * 1024 * 4;
    
    JrRuntimeStack();
    
    uint8_t* pointer;
    uint8_t data[JrRuntimeStack::Size];
    
    void push4(uint32_t value);
    uint32_t pop4();
};

// The runtime context for VM
struct JrRuntimeContext {
    JrRuntimeContext();
    ~JrRuntimeContext();
    
    JrRuntimeStack *stack;
};


#endif
