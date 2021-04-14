#ifndef __joyeer_driver_driver_h__
#define __joyeer_driver_driver_h__

#include "joyeer/compiler/compiler+service.h"
#include "joyeer/driver/arguments.h"

class Driver {
public:
    Driver(CommandLineArguments::Ptr arguments);
    void run();

private:
    CompilerService* compiler;
    CommandLineArguments::Ptr arguments;
};

#endif
