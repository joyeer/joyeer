#ifndef __joyeer_compiler_program_h__
#define __joyeer_compiler_program_h__

#include "joyeer/compiler/sourcefile.h"
#include "joyeer/compiler/compileopts.h"
#include "joyeer/compiler/context.h"

#include <unordered_map>

class Program {
public:
    Program(CompileOpts::Ptr options);
    void run(std::wstring inputfile);
    
private:
    
    // Compile an SourceFile
    void compile(SourceFile* sourcefile);
    // try to import module, and return the exported symboltable;
    SourceFile* tryImport(CompileContext::Ptr context, const std::wstring& moduleName);
    SourceFile* findSourceFile(const std::wstring& path, const std::wstring relativeFolder = L"");
    
    void debugPrint(Node::Ptr node, const std::wstring& debugFilePath);
    void debugPrint(const std::wstring& debugFilePath);
    
    CompileOpts::Ptr options;
    std::unordered_map<std::wstring, SourceFile*> sourcefiles;
};
#endif
