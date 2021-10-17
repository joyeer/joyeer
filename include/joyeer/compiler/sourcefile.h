#ifndef __joyeer_compiler_sourcefile_h__
#define __joyeer_compiler_sourcefile_h__

#include "joyeer/driver/arguments.h"
#include "joyeer/compiler/node.h"

// SourceFile contains all information of source file in disk
class SourceFile {
public:
    using Ptr = std::shared_ptr<SourceFile>;
    
public:
    SourceFile(const std::string& workingDirectory, const std::string& path);
    
    // get .joyeer file's relative location against the working directory
    [[nodiscard]] const std::string& getLocation() const {
        return pathInWorkingDirectory;
    }
    
    // get .joyeer file's abstract location
    [[nodiscard]] const std::string getAbstractLocation() const {
        return location.string();
    }
    
    // get .joyeer's parent folder
    [[nodiscard]] const std::string getParentFolder() const {
        return location.parent_path().string();
    }
    
    // file content
    std::string content;
    
    // lexer parsing result: token list
    std::vector<Token::Ptr> tokens;

    // grammar parsing result,
    // fileModule is the root node of AST
    FileModuleDecl::Ptr filemodule;

    JrModuleTypeDef::Ptr moduleClass;
    
protected:
    // the path relative to the working directory
    std::string pathInWorkingDirectory;
    
    // file location
    std::filesystem::path location;
    
    void open(const std::string& path);
};

#endif
