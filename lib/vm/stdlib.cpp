//
// Created by Qing Xu on 2021/12/1.
//

#include "joyeer/vm/stdlib.h"
#include "joyeer/vm/interpreter.h"

Value Global_$_print::operator()(Executor* executor, Arguments *args) const {
    auto value =args->getArgument(0);
    printf("%lld\n", value.intValue);
    return {
            .intValue = 0,
    };
}

Value Array_$$_get::operator()(Executor* executor, Arguments *args) const {
    auto objValue = args->getArgument(0);
    auto indexValue = args->getArgument(1);

    auto arrayClass = executor->isolateVM->arrayClass;

    auto itemValue = arrayClass->get(objValue.intValue, indexValue);
    return itemValue;
}
