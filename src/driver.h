#ifndef __joyeer_driver_driver_h__
#define __joyeer_driver_driver_h__

#include "runtime/arguments.h"
#include "compiler/compiler+service.h"
#include "vm/isolate.h"
#include "diagnostic/diagnostic.h"

class Driver {
public:
    Driver(Diagnostics* diagnostics, CommandLineArguments::Ptr arguments);
    void run();

private:
    CompilerService* compiler;
    IsolateVM* vm;
    CommandLineArguments::Ptr arguments;
    Diagnostics* diagnostics;
};

#endif
