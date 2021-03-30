#include "joyeer/compiler/sourcefile.h"
#include <iostream>
#include <sstream>
#include <fstream>


void SourceFile::open(const std::string &path) {
    location = path;
    std::ifstream wif(path);
    std::stringstream wss;
    wss << wif.rdbuf();
    
    content = wss.str();
}
