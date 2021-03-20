#include "joyeer/compiler/sourcefile.h"
#include <iostream>
#include <sstream>
#include <fstream>


void SourceFile::open(const std::wstring &path) {
    location = path;
    std::wifstream wif(path);
    std::wstringstream wss;
    wss << wif.rdbuf();
    
    content = wss.str();
}
