#include "program.h"
#include "compiler/lexparser.h"
#include "syntaxparser.h"
#include "IRGen.h"

Program::Program(CompileOpts::Ptr opts):
context(std::make_shared<CompileContext>(opts))
{
    
}

void Program::run(std::wstring inputfile) {
    auto sourcelife = findSourceFile(inputfile);
}

SourceFile* Program::findSourceFile(const std::wstring &path) {
    if(sourcefiles.find(path) == sourcefiles.end()) {
        auto sourcefile = new SourceFile();
        sourcefile->open(path);
        sourcefiles.insert({path, sourcefile});
    }
    
    return sourcefiles.find(path)->second;
}


void Program::compile(SourceFile *sourcefile) {
    LexParser lexParser();
}
