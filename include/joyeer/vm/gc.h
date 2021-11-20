//
// Created by Qing Xu on 2021/11/15.
//

#ifndef __joyeer_vm_gc_h__
#define __joyeer_vm_gc_h__

#include "joyeer/vm/heaps.h"
#include "joyeer/vm/object.h"

class GC {
public:
    JrObject* allocate(JrClass* klass);

private:
    Heap* heap { new Heap() };
};


#endif //__joyeer_vm_gc_h__
