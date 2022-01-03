//
// Created by Qing Xu on 2022/1/3.
//

#ifndef __joyeer_runtime_sys_h__
#define __joyeer_runtime_sys_h__

#include "joyeer/runtime/types.h"

struct Global_$_print : public  CMethod {
    Value operator () (Executor* executor, Arguments* args) const override;
};

struct Array_$$_get: public CMethod {
    Value operator () (Executor* executor, Arguments* args) const override;
};

struct Array_$$_set: public CMethod {
    Value operator () (Executor* executor, Arguments* args) const override;
};

struct Array_$$_size: public CMethod {
    Value operator () (Executor* executor, Arguments* args) const override;
};


#endif //__joyeer_runtime_sys_h__
