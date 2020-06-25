#ifndef __joyeer_runtime_instruction_h__
#define __joyeer_runtime_instruction_h__

#include <stdint.h>

enum Opcode {
    OP_NONE = 0x00,
    OP_ICONST,
    OP_ICONST_64,
    
    OP_LOAD,
    OP_INVOKE,
    OP_DEBUG,

};

class JrCodeWriter {
public:
    JrCodeWriter();
    ~JrCodeWriter();
    
    void write(uint8_t opcode, uint8_t size, uint8_t value[]);
};

#endif
