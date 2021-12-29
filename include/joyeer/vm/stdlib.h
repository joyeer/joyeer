//
// Created by Qing Xu on 2021/12/1.
//

#ifndef __joyeer_vm_stdlib_h__
#define __joyeer_vm_stdlib_h__

#include "joyeer/vm/types.h"

struct Global_$_print : public  CMethod {
    Value operator () (IsolateVM* vm, Arguments* args) const override;
};

struct Array_$$_append: public CMethod {
    Value operator () (IsolateVM* vm, Arguments* args) const override;
};


struct Array_$$_get: public CMethod {
    Value operator () (IsolateVM* vm, Arguments* args) const override;
};


struct Array_$$_size: public CMethod {

};


#endif //__joyeer_vm_stdlib_h__
