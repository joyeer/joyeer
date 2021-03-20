#include "joyeer/compiler/diagnostic.h"
#include <iostream>

const std::wstring Diagnostics::errorNoSuchFileOrDirectory = L"no such file or directory: ";
const std::wstring Diagnostics::errorFileImportShouldAtTopOfSourceFile = L"`fileimport` should be presented at the begin of source file";

ErrorLevel Diagnostics::errorLevel = none;

void Diagnostics::reportError(const std::wstring& message) {
    errorLevel = failure;
    std::wcout << message << std::endl;
}

void Diagnostics::reportError(ErrorLevel level, const std::wstring &error, ...) {
    std::wcout << error << std::endl;
}
