#include "diagnostic.h"
#include <iostream>

void Diagnostics::reportError(const std::wstring& message) {
    std::wcout << message << std::endl;
}
