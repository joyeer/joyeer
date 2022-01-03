//
// Created by Qing Xu on 2021/11/4.
//

#ifndef __joyeer_vm_object_h__
#define __joyeer_vm_object_h__

#include <cstdint>
#include "joyeer/runtime/types.h"

struct ObjectHead {
    int klassSlot: 20;
    int refCount: 12;
    int reversed: 32;
};

struct Object {
    ObjectHead head;
    Byte data[];
};


#endif //__joyeer_vm_object_h__
