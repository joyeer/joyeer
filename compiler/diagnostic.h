#ifndef __joyeer_compiler_diagnostic_h__
#define __joyeer_compiler_diagnostic_h__

#include <string>

enum ErrorLevel {
    none,
    // compiler progress will be continue
    report,
    // compiler will stop work
    failure
};

struct ErrorMessage {
    ErrorLevel level;
    std::wstring message;
    int lineAt;
    int columnAt;
};

struct Diagnostics {
    
    // not such file or directory
    static const std::wstring errorNoSuchFileOrDirectory;
    // `fileimport` should be presented at the begin of source file
    static const std::wstring errorFileImportShouldAtTopOfSourceFile;
    
    static ErrorLevel errorLevel;
    
    static void reportError(const std::wstring& error);
    static void reportError(ErrorLevel level, const std::wstring& error, ...);
    
};

#endif
