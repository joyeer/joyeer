#include "joyeer/compiler/diagnostic.h"
#include <iostream>

ErrorLevel Diagnostics::errorLevel = none;

void Diagnostics::reportError(const char* message) {
    errorLevel = failure;
    std::wcout << message << std::endl;
}

void Diagnostics::reportError(ErrorLevel level, const char* error, ...) {
    std::wcout << error << std::endl;
}
