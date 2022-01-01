#include "joyeer/runtime/diagnostic.h"
#include <iostream>

ErrorLevel Diagnostics::errorLevel = none;

void Diagnostics::reportError(const char* message) {
    errorLevel = failure;
    std::cout << message << std::endl;
}

void Diagnostics::reportError(ErrorLevel level, const char* error, ...) {
    std::cout << error << std::endl;
}
