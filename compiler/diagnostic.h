#ifndef __joyeer_compiler_diagnostic_h__
#define __joyeer_compiler_diagnostic_h__

#include <string>

struct Diagnostics {
    static void reportError(const std::wstring& error);
};

#endif
