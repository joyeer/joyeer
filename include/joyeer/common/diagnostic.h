#ifndef __joyeer_compiler_diagnostic_h__
#define __joyeer_compiler_diagnostic_h__

#include <string>

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
    
    // `fileimport` should be presented at the begin of source file
    DIAGNOSTICS_ERROR_MESSAGE(errorFileImportShouldAtTopOfSourceFile,           "`import` should be presented at the begin of source file")
    DIAGNOSTICS_ERROR_MESSAGE(errorNoSuchFileOrDirectory,                       "no such file or directory:")
    DIAGNOSTICS_ERROR_MESSAGE(success,                                          "message")
    
    static ErrorLevel errorLevel;
    
    static void reportError(const char* error);
    static void reportError(ErrorLevel level, const char* error, ...);
    
};

#endif
