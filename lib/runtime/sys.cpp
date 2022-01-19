//
// Created by Qing Xu on 2022/1/3.
//

#include "joyeer/runtime/sys.h"
#include "joyeer/runtime/arguments.h"
#include "joyeer/runtime/executor.h"

Value Global_$_print(Executor* executor, Arguments *args) {
    auto wrappedObj = args->getArgument(0);
    auto optionalClass = executor->vm->optionalClass;
    auto typeSlot = optionalClass->valueType(wrappedObj);
    auto type = executor->vm->types->operator[](typeSlot);
    switch (type->kind) {
        case ValueType::Int: {
            auto value = optionalClass->intValue(wrappedObj);
            printf("%lld\n", value);
        }
            break;
        case ValueType::Bool: {
            auto value = optionalClass->boolValue(wrappedObj);
            if(value) {
                printf("true\n");
            } else {
                printf("false\n");
            }
        }
            break;
        case ValueType::String: {
            auto string = executor->vm->stringClass->toString(wrappedObj);
            printf("%s\n", string.c_str());
        }
            break;
        default:
            assert(false);
    }
    return 0;
}

Value Global_$_autoWrapping_Int(Executor* executor, Arguments* args) {
    auto value = args->getArgument(0);

    auto wrapped = executor->vm->optionalClass->allocate(executor->vm,(Int)value);
    return wrapped;
}

Value Global_$_autoWrapping_Bool(Executor* executor, Arguments* args) {
    auto value = args->getArgument(0);

    auto wrapped = executor->vm->optionalClass->allocate(executor->vm, (Bool)value);
    return wrapped;
}

Value Array_$$_get(Executor* executor, Arguments *args) {
    auto indexValue = args->getArgument(0);
    auto objValue = args->getArgument(1);

    auto arrayClass = executor->vm->arrayClass;
    assert(arrayClass != nullptr);

    auto itemValue = arrayClass->get(objValue, indexValue);
    return itemValue;
}

Value Array_$$_size(Executor *executor, Arguments *args) {
    auto objValue = args->getArgument(0);
    auto arrayClass = executor->vm->arrayClass;
    assert(arrayClass != nullptr);
    auto resultValue = arrayClass->getLength(objValue);
    return resultValue;
}

Value Array_$$_set(Executor *executor, Arguments *args) {
    auto newValue = args->getArgument(0);
    auto indexValue = args->getArgument(1);
    auto objValue = args->getArgument(2);

    auto arrayClass = executor->vm->arrayClass;
    arrayClass->set(objValue, indexValue, newValue);
    assert(arrayClass != nullptr);

    return 0;
}

Value Dict_$_init(Executor* executor, Arguments* args) {
    auto obj = args->getArgument(0);
    return obj;
}

Value Dict_$$_insert(Executor* executor, Arguments* args) {
    auto value = args->getArgument(0);
    auto keyValue = args->getArgument(1);
    auto objValue = args->getArgument(2);

    auto dictClass = executor->vm->dictClass;
    dictClass->insert(objValue, keyValue, value);
    return 0;
}

Value Dict_$$_get(Executor* executor, Arguments* args) {
    auto keyValue = args->getArgument(0);
    auto objValue = args->getArgument(1);

    auto dictClass = executor->vm->dictClass;
    auto value = dictClass->get(objValue, keyValue);

    return value;
}

Value StringBuilder_$$_append(Executor* executor, Arguments* args) {
    auto value = args->getArgument(0);
    auto objValue = args->getArgument(1);

    auto stringBuilderClass = executor->vm->stringBuilderClass;
    stringBuilderClass->append(objValue, value);
    return objValue;
}

Value StringBuilder_$$_toString(Executor* executor, Arguments* args) {
    auto objValue = args->getArgument(0);
    auto stringBuilderClass = executor->vm->stringBuilderClass;
    return stringBuilderClass->toString(objValue);
}

