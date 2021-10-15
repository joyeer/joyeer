#ifndef __joyeer_compiler_lexer_lexparser_h__
#define __joyeer_compiler_lexer_lexparser_h__

#include "joyeer/compiler/sourcefile.h"
#include "joyeer/compiler/token.h"

class LexParser {
public:
    // parse and tokenize the source file
    void parse(SourceFile::Ptr sourcefile);
    
private:
    void parseStringIdentifier();
    void parseStringLiteral();
    void parseOctalLiteral(std::string::const_iterator startAt);
    void parseNumberLiteral(std::string::const_iterator startAt);
    void parseHexLiteral(std::string::const_iterator startAt);
    void pushOperator(TokenKind kind, std::string op, std::string::const_iterator startIterator);
    void parseOperator(std::string::const_iterator startIterator);
    void parsePunctuation(std::string::const_iterator startIterator);

private:
    std::string::const_iterator iterator;
    std::string::const_iterator endIterator;

    size_t lineNumber = 0;
    std::string::const_iterator lineStartAtPosition;

    // Source files
    SourceFile::Ptr sourcefile; 
};


#endif
