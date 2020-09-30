#ifndef __joyeer_compiler_program_h__
#define __joyeer_compiler_program_h__

#include "compiler/sourcefile.h"
#include "compiler/compileopts.h"
#include "compiler/context.h"

#include <unordered_map>

class Program {
public:
    Program(CompileOpts::Ptr options);
    void run(std::wstring inputfile);
private:
    
    void compile(SourceFile* sourcefile);
    
    SourceFile* findSourceFile(const std::wstring& path);
    CompileContext::Ptr context;
    
    std::unordered_map<std::wstring, SourceFile*> sourcefiles;
};
#endif
