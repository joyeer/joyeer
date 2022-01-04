//
// Created by Qing Xu on 2022/1/3.
//

#include "joyeer/runtime/sys.h"
#include "joyeer/runtime/arguments.h"
#include "joyeer/runtime/executor.h"

Value Global_$_print(Executor* executor, Arguments *args) {
    auto value = args->getArgument(0);
    printf("%lld\n", value.intValue);
    return {
            .intValue = 0,
    };
}

Value Array_$$_get(Executor* executor, Arguments *args) {
    auto objValue = args->getArgument(0);
    auto indexValue = args->getArgument(1);

    auto arrayClass = executor->isolateVM->arrayClass;

    auto itemValue = arrayClass->get(objValue.intValue, indexValue);
    return itemValue;
}

Value Array_$$_size(Executor *executor, Arguments *args) {
    auto objValue = args->getArgument(0);
    auto arrayClass = executor->isolateVM->arrayClass;
    return arrayClass->getLength(objValue.intValue);
}

Value Array_$$_set(Executor *executor, Arguments *args) {
    Value result;
    return result;
}
