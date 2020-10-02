#include "program.h"
#include "compiler/lexparser.h"
#include "compiler/diagnostic.h"
#include "compiler/binder.h"
#include "compiler/typecheck.h"
#include "syntaxparser.h"
#include "IRGen.h"

#define CHECK_ERROR_CONTINUE \
    if(Diagnostics::errorLevel != none) { \
        return; \
    }

Program::Program(CompileOpts::Ptr opts):
options(opts) {    
}

void Program::run(std::wstring inputfile) {
    auto sourcelife = findSourceFile(inputfile);
    
    compile(sourcelife);
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
    
    auto context= std::make_shared<CompileContext>(options);
    
    // lex structure analyze
    LexParser lexParser;
    lexParser.parse(sourcefile->content);
    
    CHECK_ERROR_CONTINUE
    
    // syntax analyze
    SyntaxParser syntaxParser(lexParser.tokens);
    auto block = syntaxParser.parse();
    
    CHECK_ERROR_CONTINUE
    
    // Detect for type creating
    Binder binder(context);
    binder.bind(block);
    
    //
    auto fileimports = block->getFileImports();
    
    for(std::vector<FileImportDecl::Ptr>::const_iterator iterator = fileimports.begin(); iterator != fileimports.end(); iterator ++ ) {
        auto importedFile = *iterator;
        auto importedFilename = importedFile->getImportedFilename();
        auto importfile = findSourceFile(importedFilename);
    }
    
    CHECK_ERROR_CONTINUE
    
    TypeChecker typeChecker(context);
    typeChecker.verify(std::static_pointer_cast<Node>(block));
    
    CHECK_ERROR_CONTINUE
}
