//
// Created by Qing Xu on 2021/11/11.
//

#include "isolate.h"
#include "interpreter.h"
#include "runtime/loader.h"

void InterpretedIsolatedVM::run(ModuleClass *module) {
    ClassLoader loader(this);
    loader.load(module);

    InterpretedExecutor * executor = new InterpretedExecutor(this);
    executor->execute(module);
    // delete executor;
}
