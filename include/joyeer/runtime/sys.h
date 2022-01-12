//
// Created by Qing Xu on 2022/1/3.
//

#ifndef __joyeer_runtime_sys_h__
#define __joyeer_runtime_sys_h__

#include "joyeer/runtime/types.h"

Value Global_$_print(Executor* executor, Arguments* args);


Value Array_$$_get(Executor* executor, Arguments* args);
Value Array_$$_set(Executor* executor, Arguments* args);
Value Array_$$_size(Executor* executor, Arguments* args);

Value Dict_$_init(Executor* executor, Arguments* args);
Value Dict_$$_insert(Executor* executor, Arguments* args);
Value Dict_$$_get(Executor* executor, Arguments* args);

#endif //__joyeer_runtime_sys_h__
