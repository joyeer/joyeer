#ifndef __joyeer_driver_driver_h__
#define __joyeer_driver_driver_h__

#include "joyeer/compiler/compilerservice.h"
#include "joyeer/driver/arguments.h"
class Driver {
public:
    Driver();

    void run();
private:
    CompilerService* compiler;
};

#endif
