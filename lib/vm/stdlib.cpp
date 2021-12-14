//
// Created by Qing Xu on 2021/12/1.
//

#include "joyeer/vm/stdlib.h"
#include "joyeer/vm/interpreter.h"

Value Global_$_print::operator()(IsolateVM *vm, Arguments *args) const {
    auto value =args->getArgument(0);
    printf("%lld\n", value.intValue);
    return {
            .intValue = 0,
    };
}