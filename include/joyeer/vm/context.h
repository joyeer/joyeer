//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_context_h__
#define __joyeer_vm_context_h__

#include "joyeer/vm/stringtable.h"

// VM context
struct VContext {

    explicit VContext() {
    }

    StringTable* stringTable;
};

#endif // __joyeer_vm_context_h__
