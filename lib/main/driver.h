#ifndef __joyeer_driver_driver_h__
#define __joyeer_driver_driver_h__

#include "joyeer/runtime/arguments.h"
#include "joyeer/compiler/compiler+service.h"
#include "joyeer/vm/isolate.h"


class Driver {
public:
    explicit Driver(CommandLineArguments::Ptr arguments);
    void run();

private:
    CompilerService* compiler;
    IsolateVM* vm;
    CommandLineArguments::Ptr arguments;
};

#endif
