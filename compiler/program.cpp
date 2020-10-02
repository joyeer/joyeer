#include "program.h"
#include "compiler/lexparser.h"
#include "compiler/diagnostic.h"
#include "compiler/binder.h"
#include "compiler/typecheck.h"
#include "syntaxparser.h"
#include "IRGen.h"
#include <iostream>

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

SourceFile* Program::findSourceFile(const std::wstring &path, const std::wstring relativeFolder) {
    auto sourcefile = std::filesystem::path(path);
    if(sourcefile.is_relative()) {
        // check the relative folder
        auto target = std::filesystem::path(relativeFolder) / path;
        if(std::filesystem::exists(target)) {
            sourcefile = target;
        }
    }
    
    auto sourcefilePath = sourcefile.wstring();
    if(sourcefiles.find(sourcefilePath) == sourcefiles.end()) {
        auto sourcefile = new SourceFile();
        sourcefile->open(sourcefilePath);
        sourcefiles.insert({sourcefilePath, sourcefile});
    }
    
    return sourcefiles.find(sourcefilePath)->second;
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
    debugPrint(block, sourcefile->location.wstring() + L".parser.debug.txt");
    CHECK_ERROR_CONTINUE
    
    // Detect for type creating
    Binder binder(context);
    binder.bind(block);
    debugPrint(block, sourcefile->location.wstring() + L".binder.debug.txt");
    CHECK_ERROR_CONTINUE
    
    // resolve all importfile statement in source file, and try to compile them
    auto fileimports = block->getFileImports();
    auto relativedFolder = sourcefile->location.parent_path().wstring();
    for(std::vector<FileImportDecl::Ptr>::const_iterator iterator = fileimports.begin(); iterator != fileimports.end(); iterator ++ ) {
        auto importedFile = *iterator;
        auto importedFilename = importedFile->getImportedFilename();
        auto importfile = findSourceFile(importedFilename, relativedFolder);
        
        if(importfile == nullptr) {
            Diagnostics::reportError(L"Error");
        }
        compile(importfile);
        CHECK_ERROR_CONTINUE
    }
    
    TypeChecker typeChecker(context);
    typeChecker.verify(std::static_pointer_cast<Node>(block));
    debugPrint(block, sourcefile->location.wstring() + L".typechecker.debug.txt");
    CHECK_ERROR_CONTINUE

}

void Program::debugPrint(Node::Ptr node, const std::wstring &debugFilePath) {
    if(options->vmDebug) {
        NodeDebugPrinter syntaxDebugger(debugFilePath);
        syntaxDebugger.print(node);
        syntaxDebugger.close();
    }
}