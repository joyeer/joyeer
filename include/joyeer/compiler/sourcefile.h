#ifndef __joyeer_compiler_sourcefile_h__
#define __joyeer_compiler_sourcefile_h__

#include "joyeer/driver/arguments.h"
#include "joyeer/compiler/node+types.h"
#include "joyeer/compiler/node+stmt.h"
#include "joyeer/compiler/node+expr.h"
#include "joyeer/runtime/sys/module.h"

// SourceFile contains all information of source file in disk
class SourceFile {
public:
    using Ptr = std::shared_ptr<SourceFile>;
    
public:
    SourceFile(const std::string& workingDirectory, const std::string& path);
    
    // get .joyeer file's relative location against the working directory
    const std::string& getLocation() const {
        return pathInWorkingDirectory;
    }
    
    // get .joyeer file's abstract location
    const std::string getAbstractLocation() const {
        return location.string();
    }
    
    // get .joyeer's parent folder
    const std::string getParentFolder() const {
        return location.parent_path().string();
    }
    
    // file content
    std::string content;
    
    // lexer parsing result: token list
    std::vector<Token::Ptr> tokens;

    // grammer parsing result, 
    // filemodule is the root node of AST 
    FileModuleDecl::Ptr filemodule;

    JrModuleClass* moduleClass;
    
protected:
    // the path relatived to the working directory
    std::string pathInWorkingDirectory;
    
    // file location
    std::filesystem::path location;
    
    void open(const std::string& path);
};

#endif
