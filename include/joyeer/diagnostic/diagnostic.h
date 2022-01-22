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
    ErrorMessage(ErrorLevel level, const char* message, int lineAt, int columnAt);

    ErrorLevel level;
    const char* message;
    int lineAt;
    int columnAt;
};

#define DIAGNOSTICS_ERROR_MESSAGE(name, message) static constexpr const char* name = message;

struct Diagnostics {

    explicit Diagnostics(): errors() {};
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
    void reportError(ErrorLevel level, const char* error);

    // print the error into consoles
    void printErrors();

    // print the error message
    void printError(ErrorMessage& error);

    std::vector<ErrorMessage> errors;
    
};

#endif // __joyeer_diagnostic_diagnostic_h__
