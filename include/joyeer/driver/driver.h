#ifndef __joyeer_driver_driver_h__
#define __joyeer_driver_driver_h__

#include "joyeer/driver/arguments.h"
#include "joyeer/compiler/compiler+service.h"
#include "joyeer/runtime/interpreter.h"


class Driver {
public:
    Driver(CommandLineArguments::Ptr arguments);
    void run();

private:
    CompilerService* compiler;

    JrRuntimeContext* runtimeContext;
    JrInterpreter * interpreter;
    
    CommandLineArguments::Ptr arguments;
};

#endif
