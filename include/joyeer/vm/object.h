//
// Created by Qing Xu on 2021/11/4.
//

#ifndef __joyeer_vm_object_h__
#define __joyeer_vm_object_h__

#include <cstdint>
#include "joyeer/vm/types.h"


struct Object {
    Class* klass;
    Byte data[];
};

// calculate a given Class size
static size_t calculateClassSize(Class* klass);


#endif //__joyeer_vm_object_h__
