#include "joyeer/diagnostic/diagnostic.h"
#include <iostream>

ErrorMessage::ErrorMessage(ErrorLevel level, const char* error, int lineAt, int columnAt):
level(level),
message(error),
lineAt(lineAt),
columnAt(columnAt) {

}

void Diagnostics::reportError(ErrorLevel level, int lineAt, int columnAt, const char* error, ...) {
    ErrorMessage e(level, error, -1, -1);
    errors.push_back(e);
}

void Diagnostics::reportError(ErrorLevel level, const char* error) {
    ErrorMessage e(level, error, -1, -1);
    errors.push_back(e);
}

void Diagnostics::printErrors() {
    for(auto error: errors) {
        printError(error);
    }
}

void Diagnostics::printError(ErrorMessage &error) {
    std::cout << error.message << std::endl;
}