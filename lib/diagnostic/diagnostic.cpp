#include "joyeer/diagnostic/diagnostic.h"
#include <iostream>


void Diagnostics::reportError(ErrorLevel level, int lineAt, int columnAt, const char* error, ...) {
    std::string errorString = error;
    errors.push_back({
        .level = level,
        .lineAt = lineAt,
        .columnAt = columnAt,
        .message = errorString
    });
}
