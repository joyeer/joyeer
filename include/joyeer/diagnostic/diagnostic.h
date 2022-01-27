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

    DIAGNOSTICS_ERROR_MESSAGE(errorFileImportShouldAtTopOfSourceFile,           "`import` should be presented at the begin of source file")
    DIAGNOSTICS_ERROR_MESSAGE(errorNoSuchFileOrDirectory,                       "no such file or directory:")

    /**
     * Lex error
     */
    DIAGNOSTICS_ERROR_MESSAGE(errorOctalNumberFormat,                           "Octal number only contains 0,1,2,3,4,5,6,7")

    /**
     * Syntax errors
     */

    DIAGNOSTICS_ERROR_MESSAGE(errorCannotFindFunction,                          "Cannot find '%s' in scope")
    DIAGNOSTICS_ERROR_MESSAGE(errorCannotFindType,                              "Cannot find type '%s' in scope")
    DIAGNOSTICS_ERROR_MESSAGE(errorVarDeclMissingAnnotation,                    "Type annotation missing in variable declaration")
    DIAGNOSTICS_ERROR_MESSAGE(errorNilRequireContextualType,                    "'nil' requires a contextual type")
    DIAGNOSTICS_ERROR_MESSAGE(errorNilCannotInitializeSpecifiedType,            "'nil' cannot initialize specified type '%s'")
    DIAGNOSTICS_ERROR_MESSAGE(success,                                          "success!")


    void reportError(ErrorLevel level, const char* error, ...);
    void reportError(ErrorLevel level, int lineAt, int columnAt, const char* error, ...);

    // print the error into consoles
    void printErrors();

    // print the error message
    void printError(ErrorMessage& error);

    std::vector<ErrorMessage> errors;
    
};

#endif // __joyeer_diagnostic_diagnostic_h__
