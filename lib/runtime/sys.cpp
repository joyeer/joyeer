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
    auto indexValue = args->getArgument(0);
    auto objValue = args->getArgument(1);

    auto arrayClass = executor->vm->arrayClass;
    assert(arrayClass != nullptr);

    auto itemValue = arrayClass->get(objValue.intValue, indexValue);
    return itemValue;
}

Value Array_$$_size(Executor *executor, Arguments *args) {
    auto objValue = args->getArgument(0);
    auto arrayClass = executor->vm->arrayClass;
    assert(arrayClass != nullptr);
    auto resultValue = arrayClass->getLength(objValue.intValue);
    return resultValue;
}

Value Array_$$_set(Executor *executor, Arguments *args) {
    auto newValue = args->getArgument(0);
    auto indexValue = args->getArgument(1);
    auto objValue = args->getArgument(2);

    auto arrayClass = executor->vm->arrayClass;
    arrayClass->set(objValue.intValue, indexValue, newValue);
    assert(arrayClass != nullptr);

    return {
        .intValue = 0
    };
}

Value Dict_$$_insert(Executor* executor, Arguments* args) {
    auto value = args->getArgument(0);
    auto keyValue = args->getArgument(1);
    auto objValue = args->getArgument(2);

    auto dictClass = executor->vm->dictClass;
    dictClass->insert(objValue.intValue, keyValue, value);
    return { .intValue = 0 };
}

Value Dict_$$_get(Executor* executor, Arguments* args) {
    auto keyValue = args->getArgument(0);
    auto objValue = args->getArgument(1);

    auto dictClass = executor->vm->dictClass;
    auto value = dictClass->get(objValue.intValue, keyValue);

    return value;
}

