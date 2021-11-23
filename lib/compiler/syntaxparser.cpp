#include "joyeer/compiler/syntaxparser.h"
#include "joyeer/common/diagnostic.h"
#include <cassert>

SyntaxParser::SyntaxParser(SourceFile::Ptr sourcefile):
sourcefile(sourcefile) {
    iterator = sourcefile->tokens.begin();
    endIterator = sourcefile->tokens.end();
}

FileModuleDecl::Ptr SyntaxParser::parse() {
    std::vector<std::shared_ptr<Node>> decls;
    while(iterator != endIterator) {
        std::shared_ptr<Node> decl = tryParseStmt();
        if(decl == nullptr) {
            Diagnostics::reportError("[Error]");
            return nullptr; 
        }

        decls.push_back(decl);
    }
    auto block =  std::make_shared<StmtsBlock>(decls);
    auto descriptor = std::make_shared<ModuleDescriptor>(sourcefile->getLocation());
    return std::make_shared<FileModuleDecl>(descriptor, block);
}

////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////

Node::Ptr SyntaxParser::tryParseDecl() {
    auto constantDecl = tryParseConstDecl();
    if(constantDecl != nullptr) {
        return constantDecl;
    }

    auto varDecl = tryParseVarDecl();
    if(varDecl != nullptr) {
        return varDecl;
    }

    auto classDecl = tryParseClassDecl();
    if(classDecl != nullptr) {
        return classDecl;
    }

    auto funcDecl = tryParseFuncDecl();
    if(funcDecl != nullptr) {
        return funcDecl;
    }
    
    auto initializer = tryParseConstructorDecl();
    if(initializer != nullptr) {
        return initializer;
    }
    
    auto fileimport = tryParseFileImportStatement();
    if(fileimport != nullptr) {
        return fileimport;
    }

    return nullptr;
}

Node::Ptr SyntaxParser::tryParseFuncDecl() {
    if(tryEat(TokenKind::keyword, Keywords::FUNC) == nullptr) {
        return nullptr;
    }

    auto identifier = tryParseIdentifierExpr();
    if(identifier == nullptr) {
        Diagnostics::reportError("[Error] Function except a name");
        return nullptr;
    }

    auto parameterClause = tryParseParameterClause();
    if(parameterClause == nullptr) {
        Diagnostics::reportError("[Error] Function except a parameters");
        return nullptr;
    }

    auto returnType = tryParseTypeAnnotation();
    
    auto codeBlock = tryParseStmtsBlock();
    if(codeBlock == nullptr) {
        Diagnostics::reportError("[Error] Function except code block");
        return nullptr;
    }
    
    return std::make_shared<FuncDecl>(identifier, parameterClause, returnType, codeBlock);
}

Node::Ptr SyntaxParser::tryParseConstructorDecl() {
    if(tryEat(TokenKind::keyword, Keywords::INIT) == nullptr) {
        return nullptr;
    }
    
    auto parameterClause = tryParseParameterClause();
    if(parameterClause == nullptr) {
        return nullptr;
    }

    auto codeBlock = tryParseStmtsBlock();
    if(codeBlock == nullptr) {
        Diagnostics::reportError("Error");
        return nullptr;
    }
    
    return FuncDecl::makeConstructor(parameterClause, codeBlock);
}

Node::Ptr SyntaxParser::tryParseFileImportStatement() {
    if(tryEat(TokenKind::keyword, Keywords::FILEIMPORT) == nullptr) {
        return nullptr;
    }
    
    auto literal = tryParseLiteral();
    if(literal == nullptr || literal->kind != stringLiteral) {
        Diagnostics::reportError("Error");
    }
    
    return std::make_shared<FileImportStmt>(literal);
}

Node::Ptr SyntaxParser::tryParseParameterClause() {
    if (tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        return nullptr;
    }
    // parse the parameters declaration in function
    std::vector<Pattern::Ptr> parameters;
    int i = 0 ;
    while(true) {
        Token::Ptr comma = nullptr;
        if( i > 0 ) {
            comma = tryEat(TokenKind::punctuation, Punctuations::COMMA);
        }
        
        auto identifier = tryParsePattern();
        
        if( i > 0) {
            if(identifier == nullptr && comma != nullptr) {
                Diagnostics::reportError("Error");
                return nullptr; //TODO: report an grammar error
            }
            
            if(identifier != nullptr && comma == nullptr ) {
                Diagnostics::reportError("Error");
                return nullptr; //TODO: report an grammar error
            }
        }
        
        if(identifier == nullptr) {
            break;
        }

        parameters.push_back(identifier);
        
        i++;
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_ROUND_BRACKET) == nullptr) {
        Diagnostics::reportError("Error");
        return nullptr;
    }
    
    return std::shared_ptr<Node>(new ParameterClause(parameters));
}

Node::Ptr SyntaxParser::tryParseConstDecl() {
    if (tryEat(TokenKind::keyword, Keywords::LET) == nullptr) {
        return nullptr; // Not a `let` declaration
    }

    auto pattern = tryParsePattern();
    if (pattern == nullptr) {
        Diagnostics::reportError("Error");
        return nullptr; 
    }
    
    Node::Ptr initializer = nullptr;
    if(tryEat(TokenKind::operators, Operators::EQUALS) != nullptr) {
        initializer = tryParseExpr();
        if(initializer == nullptr) {
            Diagnostics::reportError("[Error] declaration must have initializer");
        }
    }
    return std::make_shared<VarDecl>(pattern, initializer);
}

Node::Ptr SyntaxParser::tryParseVarDecl() {
    if(tryEat(TokenKind::keyword, Keywords::VAR) == nullptr) {
        return nullptr; // Not a 'var' declaration
    }

    Pattern::Ptr pattern = tryParsePattern();
    if (pattern == nullptr) {
        Diagnostics::reportError("[Error]");
       return nullptr; //TODO: report an syntax Error
    }
    Node::Ptr initializer = nullptr;
    if(tryEat(TokenKind::operators, Operators::EQUALS) != nullptr) {
        initializer = tryParseExpr();
        if(initializer == nullptr) {
            Diagnostics::reportError("[Error] declaration must have initializer");
        }
    }
    
    return std::make_shared<VarDecl>(pattern, initializer);
}

Node::Ptr SyntaxParser::tryParseClassDecl() {
    if (tryEat(TokenKind::keyword, Keywords::CLASS) == nullptr) {
        return nullptr;
    }

    auto className = tryEat(TokenKind::identifier);
    if (className == nullptr ) {
        Diagnostics::reportError("[Error]");
        return nullptr;
    }

    if (tryEat(TokenKind::punctuation, Punctuations::OPEN_CURLY_BRACKET) == nullptr) {
        Diagnostics::reportError("[Error]");
        return nullptr;
    }

    std::vector<Node::Ptr> members;
    while (true) {
        auto member = tryParseDecl();
        if(member == nullptr) {
            break;
        }
        members.push_back(member);
    }
    
    if (tryEat(TokenKind::punctuation, Punctuations::CLOSE_CURLY_BRACKET) == nullptr) {
        Diagnostics::reportError("[Error]");
        return nullptr;
    }

    return std::make_shared<ClassDecl>(className, std::make_shared<StmtsBlock>(members));
}

StmtsBlock::Ptr SyntaxParser::tryParseStmtsBlock() {
    // Code block
    if (tryEat(TokenKind::punctuation, Punctuations::OPEN_CURLY_BRACKET) == nullptr) {
        return nullptr; 
    }

    std::vector<std::shared_ptr<Node>> statements;
    
    while(true) {
        std::shared_ptr<Node> statement = tryParseStmt();
        if(statement == nullptr) {
            break;
        }
        statements.push_back(statement);
    }
    
    if (tryEat(TokenKind::punctuation, Punctuations::CLOSE_CURLY_BRACKET) == nullptr) {
        Diagnostics::reportError("Error");
        return nullptr; 
    }
    
    return std::make_shared<StmtsBlock>(statements);
}

Node::Ptr SyntaxParser::tryParseStmt() {
    auto expr = tryParseExpr();
    if(expr != nullptr) {
        return expr;
    }

    auto decl = tryParseDecl();
    if(decl != nullptr) {
        return decl;
    }

    auto loopStatement = tryParseLoopStmt();
    if(loopStatement != nullptr) {
        return loopStatement;
    }
    
    auto branchStatement = tryParseBranchStmt();
    if(branchStatement != nullptr) {
        return branchStatement;
    }

    auto returnStatement = tryParseReturnStmt();
    if(returnStatement != nullptr) {
        return returnStatement;
    }
    
    auto whileStatement = tryparseWhileStmt();
    if(whileStatement != nullptr) {
        return whileStatement;
    }
    
    return nullptr;
}

Node::Ptr SyntaxParser::tryParseLoopStmt() {
    if(tryEat(TokenKind::keyword, Keywords::FOR) == nullptr) {
        return nullptr;
    }

    auto pattern = tryParsePattern();
    if(pattern == nullptr) {
        Diagnostics::reportError("Error");
        return nullptr; // TODO: report an error;
    }

    if(tryEat(TokenKind::keyword, Keywords::IN) == nullptr) {
        Diagnostics::reportError("Error");
        return nullptr; // TODO: report a grammar error
    }

    auto expr = tryParseExpr();
    if(expr == nullptr) {
        Diagnostics::reportError("Error");
    }

    auto codeBlock = tryParseStmtsBlock();

    return std::shared_ptr<Node>(std::make_shared<ForInStmt>(pattern, expr, codeBlock));
}

Node::Ptr SyntaxParser::tryparseWhileStmt() {
    if(tryEat(TokenKind::keyword, Keywords::WHILE) == nullptr) {
        return nullptr;
    }
    
    auto expr = tryParseExpr();
    if(expr == nullptr) {
        Diagnostics::reportError("Error");
        return nullptr;
    }
    
    auto block = tryParseStmtsBlock();
    if(block == nullptr) {
        Diagnostics::reportError("Error");
        return nullptr;
    }
    
    return std::make_shared<WhileStmt>(expr, block);
}

Node::Ptr SyntaxParser::tryParseBranchStmt() {
    return tryParseIfStmt();
}

Node::Ptr SyntaxParser::tryParseIfStmt() {
    if(tryEat(TokenKind::keyword, Keywords::IF) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Node> condition = tryParseExpr();
    if(condition == nullptr) {
        return nullptr; //TODO: Report an grammar error
    }

    std::shared_ptr<Node> codeBlock = tryParseStmtsBlock();
    if(codeBlock == nullptr) {
        return nullptr; // TODO: Report a grammar error
    }

    if(tryEat(TokenKind::keyword, Keywords::ELSE) == nullptr) {
        return std::shared_ptr<Node>(new IfStmt(condition, codeBlock, nullptr));
    } else {
        std::shared_ptr<Node> elseIfStatement = tryParseIfStmt();
        if(elseIfStatement != nullptr) {
            return std::shared_ptr<Node>(new IfStmt(condition, codeBlock, elseIfStatement));
        }

        std::shared_ptr<Node> elseStatement = tryParseStmtsBlock();
        if(elseStatement == nullptr) {
            return nullptr; // TODO: report an error, miss an else code block
        }
        
        return std::shared_ptr<Node>(new IfStmt(condition, codeBlock, elseStatement));
    }
}

Node::Ptr SyntaxParser::tryParseReturnStmt() {
    if(tryEat(TokenKind::keyword, Keywords::RETURN) == nullptr) {
        return nullptr;
    }
    
    auto expr = tryParseExpr();
    return std::static_pointer_cast<Node>(std::make_shared<ReturnStmt>(expr));
}

Pattern::Ptr SyntaxParser::tryParsePattern() {
    
    auto identifier = tryParseIdentifierExpr();
    if (identifier == nullptr) {
        return nullptr;
    }
    
    auto type = tryParseTypeAnnotation();
    return std::make_shared<Pattern>(identifier, type);
}

Node::Ptr SyntaxParser::tryParseTypeAnnotation() {
    if(tryEat(TokenKind::punctuation, Punctuations::COLON) == nullptr) {
        return nullptr;
    }
    
    auto type = tryParseType();
    if(type == nullptr ) {
        Diagnostics::reportError("Error");
        return nullptr;
    }
    return type;
}


Node::Ptr SyntaxParser::tryParseType() {
    Node::Ptr type = nullptr;
    type = tryParseTypeIdentifier();
    if(type != nullptr) {
        return type;
    }
    
    type = tryParseTypeArray();
    if(type != nullptr) {
        return type;
    }
    
    return nullptr;
}

Node::Ptr SyntaxParser::tryParseTypeIdentifier() {
    auto type = tryParseIdentifierExpr();
    if(type == nullptr) {
        return nullptr;
    }
    return std::make_shared<TypeIdentifier>(type);
}

Node::Ptr SyntaxParser::tryParseTypeArray() {
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_SQUARE_BRACKET) == nullptr) {
        return nullptr;
    }
    
    auto type = tryParseType();
    if( type == nullptr) {
        Diagnostics::reportError("Error");
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_SQUARE_BRACKET) == nullptr) {
        return nullptr;
    }
    
    return std::make_shared<ArrayType>(type);
}

// expression -> prefix-expression binary-expressions /opt/
Node::Ptr SyntaxParser::tryParseExpr() {
    std::shared_ptr<Node> expr = tryParsePrefixExpr();
    if(expr == nullptr) {
        return nullptr;
    }
    
    std::vector<Node::Ptr> binaries;
    
    while(true) {
        std::shared_ptr<Node> binExpr = tryParseBinaryExpr();
        if(binExpr == nullptr) {
            break;
        }
        binaries.push_back(binExpr);
    }
    
    if(binaries.size() == 0) {
        return expr;
    }
    
    
    return std::make_shared<Expr>(expr, binaries);
}

std::shared_ptr<Node> SyntaxParser::tryParseBinaryExpr() {
    std::shared_ptr<Token> assignmentOperator = tryEat(TokenKind::operators, Operators::EQUALS);
    if(assignmentOperator != nullptr) {
        std::shared_ptr<Node> prefixExpr = tryParseExpr();
        if(prefixExpr == nullptr) {
            Diagnostics::reportError("Error");
            return nullptr;
        }
        return std::make_shared<AssignExpr>(prefixExpr);
    }
    
    auto binaryOperator = tryParseOperatorExpr();
    if(binaryOperator != nullptr) {
        std::shared_ptr<Node> prefixExpr = tryParsePrefixExpr();
        if(prefixExpr == nullptr) {
            Diagnostics::reportError("Error");
            return nullptr;
        }
        return std::make_shared<BinaryExpr>(binaryOperator, prefixExpr);
    }
    return nullptr;
}

std::shared_ptr<Node> SyntaxParser::tryParsePrefixExpr() {
    auto op = tryParseOperatorExpr();
    
    auto postfixExpr = tryParsePostfixExpr();
    if(postfixExpr == nullptr) {
        // revert the prefix operator
        if(op != nullptr) {
            previous();
        }
        return nullptr;
    }

    if(op == nullptr) {
        return postfixExpr;
    } else {
        return std::make_shared<PrefixExpr>(op, postfixExpr);
    }
}

Node::Ptr SyntaxParser::tryParsePostfixExpr(Node::Ptr postfixExpr) {
    Node::Ptr result = postfixExpr;
    
    if(postfixExpr == nullptr) {
        auto primaryExpr = tryParsePrimaryExpr();
        if(primaryExpr == nullptr) {
            return postfixExpr;
        }
        auto postfixOperator = tryParsePostfixOperatorExpr();
        if(postfixOperator == nullptr) {
            result = primaryExpr;
        } else {
            result = std::make_shared<PostfixExpr>(primaryExpr, postfixOperator);
        }
    }
    
    auto funcCallExpr = tryParseFuncCallExpr(result);
    if(funcCallExpr != nullptr) {
        auto expr = tryParsePostfixExprWithPostfixOperator(funcCallExpr);
        return tryParsePostfixExpr(expr);
    }
    
    auto subscriptExpr = tryParseSubscriptExpr(result);
    if(subscriptExpr != nullptr) {
        auto expr = tryParsePostfixExprWithPostfixOperator(subscriptExpr);
        return tryParsePostfixExpr(expr);
    }
    
    auto memberAccessExpr = tryParseMemberAccessExpr(result);
    if(memberAccessExpr != nullptr) {
        auto expr = tryParsePostfixExprWithPostfixOperator(memberAccessExpr);
        return tryParsePostfixExpr(expr);
    }
    
    return result;
}

Node::Ptr SyntaxParser::tryParsePostfixExprWithPostfixOperator(Node::Ptr postfixExpr) {
    assert(postfixExpr != nullptr);
    auto postfixOperator = tryParsePostfixOperatorExpr();
    if(postfixOperator == nullptr) {
        return postfixExpr;
    } else {
        return std::make_shared<PostfixExpr>(postfixExpr, postfixOperator);
    }
}

Node::Ptr SyntaxParser::tryParseMemberAccessExpr(Node::Ptr postfixExpr) {
    assert(postfixExpr != nullptr);
    
    if(tryEat(TokenKind::punctuation, Punctuations::DOT) == nullptr) {
        return nullptr;
    }
    
    auto identifierExpr = tryParseIdentifierExpr();
    if(identifierExpr == nullptr) {
        Diagnostics::reportError("[Error]");
        return nullptr;
    }
    
    return std::make_shared<MemberAccessExpr>(postfixExpr, identifierExpr);
}

Node::Ptr SyntaxParser::tryParseFuncCallExpr(Node::Ptr postfixExpr) {
    std::vector<std::shared_ptr<Token>>::const_iterator mark = iterator;
    
    auto identifier = postfixExpr;
    
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        iterator = mark;
        return nullptr;
    }
    
    std::vector<ArguCallExpr::Ptr> arguments;
    auto arguCall = tryParseArguCallExpr();
    if(arguCall != nullptr) {
        arguments.push_back(arguCall);
        while (true) {
            if(tryEat(TokenKind::punctuation, Punctuations::COMMA) == nullptr) {
                break;
            }
            
            auto arguCall = tryParseArguCallExpr();
            if(arguCall == nullptr) {
                Diagnostics::reportError("[Error]");
                return nullptr; //TODO: Report a grammar error
            }
            
            arguments.push_back(arguCall);
        }
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_ROUND_BRACKET) == nullptr) {
        Diagnostics::reportError("[Error]");
        return nullptr; // TODO: report an grammar error
    }
    
    return std::make_shared<FuncCallExpr>(identifier, arguments);
}

Node::Ptr SyntaxParser::tryParseSubscriptExpr(Node::Ptr postfixExpr) {
    assert(postfixExpr != nullptr);

    
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_SQUARE_BRACKET) == nullptr) {
        return nullptr;
    }
    auto expr = tryParseExpr();
    if(expr == nullptr) {
        Diagnostics::reportError("[Error] in array access");
        return nullptr;
    }
    
    if(tryEat(punctuation, Punctuations::CLOSE_SQUARE_BRACKET) == nullptr) {
        Diagnostics::reportError("[Error] in array access");
        return nullptr;
    }
    
    return std::make_shared<SubscriptExpr>(postfixExpr, expr);
    
}

ArguCallExpr::Ptr SyntaxParser::tryParseArguCallExpr() {
    auto identifier = tryParseIdentifierExpr();
    if(identifier == nullptr) {
        return nullptr;
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::COLON) == nullptr) {
        Diagnostics::reportError("[Error]");
        return nullptr; //TODO: report an grammar error
    }
    
    auto expr = tryParseExpr();
    if(expr == nullptr) {
        Diagnostics::reportError("[Error]");
        return nullptr; // TODO: report a grammar error
    }
    
    return std::make_shared<ArguCallExpr>(identifier, expr);
}


Node::Ptr SyntaxParser::tryParsePrimaryExpr() {
    auto identifier = tryParseIdentifierExpr();
    if (identifier != nullptr) {
        return identifier;
    }

    auto literalExpr = tryParseLiteralExpr();
    if (literalExpr != nullptr) {
        return literalExpr;
    }
    
    auto selfExpr = tryParseSelfExpr();
    if(selfExpr != nullptr) {
        return selfExpr;
    }
    
    auto parenthesizedExpr = tryParseParenthesizedExpr();
    if(parenthesizedExpr != nullptr) {
        return parenthesizedExpr;
    }

    return nullptr;
}

Node::Ptr SyntaxParser::tryParseSelfExpr() {
    if(tryEat(TokenKind::keyword, Keywords::SELF) == nullptr) {
        return nullptr;
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::DOT) == nullptr) {
        return std::shared_ptr<Node>(new SelfExpr(nullptr));
    }
    
    auto identifier = tryParseIdentifierExpr();
    if(identifier == nullptr) {
        Diagnostics::reportError("[Error]");
        return nullptr; //TODO: Report n error
    }
    return std::shared_ptr<Node>(new SelfExpr(identifier));
}

Node::Ptr SyntaxParser::tryParseLiteralExpr() {
    auto literal = tryParseLiteral();
    if (literal != nullptr) {
        return std::make_shared<LiteralExpr>(literal);
    }
    
    auto arrayLiteral = tryParseArrayOrDictLiteralExpr();
    if(arrayLiteral != nullptr) {
        return arrayLiteral;
    }
    return nullptr;
}

Node::Ptr SyntaxParser::tryParseArrayOrDictLiteralExpr() {
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_SQUARE_BRACKET) == nullptr) {
        return nullptr;
    }
    
    std::vector<Node::Ptr> arrayItems;
    std::vector<std::tuple<Node::Ptr, Node::Ptr>> dictItems;
    
    // parse the empty dictory literal case - [:]
    if (tryEat(TokenKind::punctuation, Punctuations::COLON)) {
        if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_SQUARE_BRACKET) == nullptr) {
            Diagnostics::reportError("[Error] Dict literal except expression here");
        }
        return std::make_shared<DictLiteralExpr>(dictItems);
    }
    
    auto keyItem = tryParseExpr();
    Node::Ptr valueItem = nullptr;
    
    bool isDict  = false;
    if(tryEat(TokenKind::punctuation, Punctuations::COLON)) {
        valueItem = tryParseExpr();
        if(valueItem == nullptr) {
            Diagnostics::reportError("[Error] Dict literal except expression here");
            return nullptr;
        }
        isDict = true;
    }
    
    if(keyItem !=  nullptr) {
        
        if(isDict) {
            dictItems.push_back(std::make_tuple(keyItem, valueItem));
        } else {
            arrayItems.push_back(keyItem);
        }
        
        while (true) {
            if(tryEat(TokenKind::punctuation, Punctuations::COMMA) == nullptr) {
                break;
            }
            
            keyItem = tryParseExpr();
            if(keyItem == nullptr) {
                Diagnostics::reportError("[Error] Array literal except expression here");
                break;
            }
            
            if(isDict) {
                if(tryEat(TokenKind::punctuation, Punctuations::COLON) == nullptr) {
                    Diagnostics::reportError("[Error] Dict literal except expression here");
                    break;
                }
                valueItem = tryParseExpr();
                if(valueItem == nullptr) {
                    Diagnostics::reportError("[Error] Dict literal except expression here");
                    break;
                }
            }
            
            if(isDict) {
                dictItems.push_back(std::make_tuple(keyItem, valueItem));
            } else {
                arrayItems.push_back(keyItem);
            }
        }
    }
    

    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_SQUARE_BRACKET) == nullptr) {
        Diagnostics::reportError("[Error] Array literal except expression here");
    }
    
    if(isDict) {
        return std::make_shared<DictLiteralExpr>(dictItems);
    } else {
        return std::make_shared<ArrayLiteralExpr>(arrayItems);
    }
    
}

Node::Ptr SyntaxParser::tryParseParenthesizedExpr() {
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Node> expr = tryParseExpr();
    if(expr == nullptr) {
        Diagnostics::reportError("[Error] Parenthesized expr error 1");
        return nullptr;
    }

    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_ROUND_BRACKET) == nullptr) {
        Diagnostics::reportError("[Error] Parenthesized expr error 2");
        return nullptr; 
    }

    return std::make_shared<ParenthesizedExpr>(expr);
}

OperatorExpr::Ptr SyntaxParser::tryParseOperatorExpr() {
    auto token = tryEat(TokenKind::operators);
    if(token == nullptr) {
        return nullptr;
    }
    return std::make_shared<OperatorExpr>(token);
}

OperatorExpr::Ptr SyntaxParser::tryParsePostfixOperatorExpr() {
    auto token = tryEat(TokenKind::operators, Operators::QUESTION);
    if(token != nullptr) {
        return std::make_shared<OperatorExpr>(token);
    }

    auto pointToken = tryEat(TokenKind::operators, Operators::POINT);
    if(pointToken != nullptr) {
        return std::make_shared<OperatorExpr>(pointToken);
    }

    return nullptr;
}

IdentifierExpr::Ptr SyntaxParser::tryParseIdentifierExpr() {
    auto identifier = tryEat(TokenKind::identifier);
    if(identifier == nullptr) {
        return nullptr;
    }
    
    return std::make_shared<IdentifierExpr>(identifier);
}

std::shared_ptr<Token> SyntaxParser::tryParseLiteral()
{
    std::shared_ptr<Token> literal = tryEat(TokenKind::stringLiteral);
    if (literal != nullptr) {
        return literal;
    }

    literal = tryEat(TokenKind::decimalLiteral);
    if (literal != nullptr) {
        return literal;
    }

    literal = tryEat(TokenKind::floatLiteral);
    if (literal != nullptr) {
        return literal;
    }

    literal = tryEat(TokenKind::nilLiteral);
    if (literal != nullptr) {
        return literal;
    }

    literal = tryEat(TokenKind::booleanLiteral);
    if (literal != nullptr)
    {
        return literal;
    }
    
    literal = tryEat(TokenKind::nilLiteral);
    if(literal != nullptr) {
        return literal;
    }

    return nullptr;
}

Token::Ptr SyntaxParser::tryEat(TokenKind kind) {
    if (iterator != endIterator) {
        auto pToken = curToken();
        if (pToken->kind == kind) {
            iterator++;
            return pToken;
        }
    }

    return nullptr;
}

Token::Ptr SyntaxParser::tryEat(TokenKind kind, const std::string &value) {
    auto pToken = tryEat(kind);
    if(pToken == nullptr) {
        return nullptr;
    }
    
    if (pToken->rawValue == value) {
        return pToken;
    }

    previous();
    return nullptr;
}

void SyntaxParser::previous() {
    iterator --;
}

Token::Ptr SyntaxParser::curToken() const {
    return *iterator;
}
