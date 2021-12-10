//
// Created by Qing Xu on 2021/12/1.
//

#include "joyeer/vm/stdlib.h"

Value Global_$_print::operator()(IsolateVM *vm, Arguments *args) const {
    return {
            .intValue = 0,
    };
}