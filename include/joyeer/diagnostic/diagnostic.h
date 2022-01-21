#ifndef __joyeer_diagnostic_diagnostic_h__
#define __joyeer_diagnostic_diagnostic_h__

#include <string>
#include <vector>

enum ErrorLevel {
    none,
    // compiler progress will be continuing
    report,
    // compiler will stop work
    failure
};

struct ErrorMessage {
    ErrorLevel level;
    std::string message;
    int lineAt;
    int columnAt;
};

#define DIAGNOSTICS_ERROR_MESSAGE(name, message) static constexpr const char* name = message;

struct Diagnostics {

    /**
     * Lex error
     */

    /**
     * Syntax errors
     */
    DIAGNOSTICS_ERROR_MESSAGE(errorFileImportShouldAtTopOfSourceFile,           "`import` should be presented at the begin of source file")
    DIAGNOSTICS_ERROR_MESSAGE(errorNoSuchFileOrDirectory,                       "no such file or directory:")
    DIAGNOSTICS_ERROR_MESSAGE(success,                                          "success!")

    void reportError(ErrorLevel level, int lineAt, int columnAt, const char* error, ...);

    std::vector<ErrorMessage> errors {};
    
};

#endif // __joyeer_diagnostic_diagnostic_h__
