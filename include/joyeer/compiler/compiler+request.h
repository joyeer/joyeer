#ifndef __joyeer_compiler_request_h__
#define __joyeer_compiler_request_h__


enum class RequestKind {
    importFileModule
};

class Request {
protected:
    RequestKind kind;
};

class ImportFileModuleRequest: Request {

};
#endif
