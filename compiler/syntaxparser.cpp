#include "syntaxparser.h"
#include "diagnostic.h"


SyntaxParser::SyntaxParser(const std::vector<std::shared_ptr<Token>> &tokens) : tokens(tokens)
{
    iterator = tokens.begin();
    endIterator = tokens.end();
}

SourceBlock::Pointer SyntaxParser::parse() {
    std::vector<std::shared_ptr<Node>> decls;
    while(iterator != endIterator) {
        std::shared_ptr<Node> decl = tryParseStatement();
        if(decl == nullptr) {
            return nullptr; // TODO: report an grammar error;
        }

        decls.push_back(decl);
    }
    
    return std::make_shared<SourceBlock>(decls);
}

////////////////////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////////////////////

Node::Pointer SyntaxParser::tryParseDecl() {
    std::shared_ptr<Node> constantDecl = tryParseConstDecl();
    if(constantDecl != nullptr) {
        return constantDecl;
    }

    std::shared_ptr<Node> varDecl = tryParseVarDecl();
    if(varDecl != nullptr) {
        return varDecl;
    }

    std::shared_ptr<Node> classDecl = tryParseClassDecl();
    if(classDecl != nullptr) {
        return classDecl;
    }

    std::shared_ptr<Node> funcDecl = tryParseFunctionDecl();
    if(funcDecl != nullptr) {
        return funcDecl;
    }
    
    std::shared_ptr<Node> initializer = tryParseConstructorDecl();
    if(initializer != nullptr) {
        return initializer;
    }

    return nullptr;
}

Node::Pointer SyntaxParser::tryParseFunctionDecl() {
    if(tryEat(TokenKind::keyword, Keywords::FUNC) == nullptr) {
        return nullptr;
    }

    auto identifier = tryParseIdentifierExpr();
    if(identifier == nullptr) {
        Diagnostics::reportError(L"[Error] Function execpt a name");
        return nullptr;
    }

    auto parameterClause = tryParseParameterClause();
    if(parameterClause == nullptr) {
        Diagnostics::reportError(L"[Error] Function execpt a parameters");
        return nullptr;
    }

    auto returnType = tryParseTypeAnnotation();
    
    auto codeBlock = tryParseCodeBlock();
    if(codeBlock == nullptr) {
        Diagnostics::reportError(L"[Error] Function execpt code block");
        return nullptr;
    }
    
    return std::make_shared<FuncDecl>(identifier, parameterClause, returnType, codeBlock);
}

std::shared_ptr<Node> SyntaxParser::tryParseConstructorDecl() {
    if(tryEat(TokenKind::keyword, Keywords::INIT) == nullptr) {
        return nullptr;
    }
    
    std::shared_ptr<Node> parameterClause = tryParseParameterClause();
    if(parameterClause == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Node> codeBlock = tryParseCodeBlock();
    if(codeBlock == nullptr) {
        return nullptr; // TODO: Error
    }

    return std::shared_ptr<Node>(new ConstructorDecl(parameterClause, codeBlock));
}

std::shared_ptr<Node> SyntaxParser::tryParseParameterClause() {
    if (tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        return nullptr;
    }
    // parse the parameters declaration in function
    std::vector<Pattern::Pointer> parameters;
    int i = 0 ;
    while(true) {
        std::shared_ptr<Token> comma = nullptr;
        if( i > 0 ) {
            comma = tryEat(TokenKind::punctuation, Punctuations::COMMA);
        }
        
        auto identifier = tryParsePattern();
        
        if( i > 0) {
            if(identifier == nullptr && comma != nullptr) {
                return nullptr; //TODO: report an grammar error
            }
            
            if(identifier != nullptr && comma == nullptr ) {
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
        return nullptr; // TODO: report a error
    }
    
    return std::shared_ptr<Node>(new ParameterClause(parameters));
}

std::shared_ptr<Node> SyntaxParser::tryParseConstDecl() {
    if (tryEat(TokenKind::keyword, Keywords::LET) == nullptr) {
        return nullptr; // Not a `let` declaration
    }

    Pattern::Pointer pattern = tryParsePattern();
    if (pattern == nullptr) {
        return nullptr; //TODO: report an syntax Error
    }
    
    Node::Pointer initializer = nullptr;
    if(tryEat(TokenKind::operators, Operators::EQULAS) != nullptr) {
        initializer = tryParseExpr();
        if(initializer == nullptr) {
            Diagnostics::reportError(L"[Error] declaration must have initializer");
        }
    }

    return std::shared_ptr<Node>(new LetDecl(pattern, initializer));
}

std::shared_ptr<Node> SyntaxParser::tryParseVarDecl() {
    if(tryEat(TokenKind::keyword, Keywords::VAR) == nullptr) {
        return nullptr; // Not a 'var' declaration
    }

    Pattern::Pointer pattern = tryParsePattern();
    if (pattern == nullptr) {
       return nullptr; //TODO: report an syntax Error
    }
    Node::Pointer initializer = nullptr;
    if(tryEat(TokenKind::operators, Operators::EQULAS) != nullptr) {
        initializer = tryParseExpr();
        if(initializer == nullptr) {
            Diagnostics::reportError(L"[Error] declaration must have initializer");
        }
    }
    
    return std::shared_ptr<Node>(new VarDecl(pattern, initializer));
}

std::shared_ptr<Node> SyntaxParser::tryParseClassDecl() {
    if (tryEat(TokenKind::keyword, Keywords::CLASS) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Token> className = tryEat(TokenKind::identifier);
    if (className == nullptr ) {
        return nullptr; // TODO: Error
    }

    if (tryEat(TokenKind::punctuation, Punctuations::OPEN_CURLY_BRACKET) == nullptr) {
        return nullptr; // TODO: Error
    }

    std::vector<std::shared_ptr<Node>> members;
    while (true) {
        std::shared_ptr<Node> member = tryParseDecl();
        if(member == nullptr) {
            break;
        }
        members.push_back(member);
    }
    
    if (tryEat(TokenKind::punctuation, Punctuations::CLOSE_CURLY_BRACKET) == nullptr) {
        return nullptr; // TODO: Error
    }

    return std::shared_ptr<Node>(new ClassDecl(className, members));
}

std::shared_ptr<Node> SyntaxParser::tryParseCodeBlock() {
    // Code block
    if (tryEat(TokenKind::punctuation, Punctuations::OPEN_CURLY_BRACKET) == nullptr) {
        return nullptr; 
    }

    std::vector<std::shared_ptr<Node>> statements;
    
    while(true) {
        std::shared_ptr<Node> statement = tryParseStatement();
        if(statement == nullptr) {
            break;
        }
        statements.push_back(statement);
    }
    
    if (tryEat(TokenKind::punctuation, Punctuations::CLOSE_CURLY_BRACKET) == nullptr) {
        return nullptr; 
    }

    return std::shared_ptr<Node>(new CodeBlock(statements));
}

Node::Pointer SyntaxParser::tryParseStatement() {
    auto expr = tryParseExpr();
    if(expr != nullptr) {
        return expr;
    }

    auto decl = tryParseDecl();
    if(decl != nullptr) {
        return decl;
    }

    auto loopStatement = tryParseLoopStatement();
    if(loopStatement != nullptr) {
        return loopStatement;
    }
    
    auto branchStatement = tryParseBranchStatement();
    if(branchStatement != nullptr) {
        return branchStatement;
    }

    auto returnStatement = tryParseReturnStatement();
    if(returnStatement != nullptr) {
        return returnStatement;
    }
    
    return nullptr;
}

std::shared_ptr<Node> SyntaxParser::tryParseLoopStatement() {
    if(tryEat(TokenKind::keyword, Keywords::FOR) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Node> pattern = tryParsePattern();
    if(pattern == nullptr) {
        return nullptr; // TODO: report an error;
    }

    if(tryEat(TokenKind::keyword, Keywords::IN) == nullptr) {
        return nullptr; // TODO: report a grammar error
    }

    std::shared_ptr<Node> expr = tryParseExpr();
    if(expr == nullptr) {

    }

    std::shared_ptr<Node> codeBlock = tryParseCodeBlock();

    return std::shared_ptr<Node>(std::make_shared<ForInStatement>(pattern, expr, codeBlock));
}

std::shared_ptr<Node> SyntaxParser::tryParseBranchStatement() {
    return tryParseIfStatement();
}

std::shared_ptr<Node> SyntaxParser::tryParseIfStatement() {
    if(tryEat(TokenKind::keyword, Keywords::IF) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Node> condition = tryParseExpr();
    if(condition == nullptr) {
        return nullptr; //TODO: Report an grammar error
    }

    std::shared_ptr<Node> codeBlock = tryParseCodeBlock();
    if(codeBlock == nullptr) {
        return nullptr; // TODO: Report a grammar error
    }

    if(tryEat(TokenKind::keyword, Keywords::ELSE) == nullptr) {
        return std::shared_ptr<Node>(new IfStatement(condition, codeBlock, nullptr));
    } else {
        std::shared_ptr<Node> elseIfStatement = tryParseIfStatement();
        if(elseIfStatement != nullptr) {
            return std::shared_ptr<Node>(new IfStatement(condition, codeBlock, elseIfStatement));
        }

        std::shared_ptr<Node> elseStatement = tryParseCodeBlock();
        if(elseStatement == nullptr) {
            return nullptr; // TODO: report an error, miss an else code block
        }
        return std::shared_ptr<Node>(new IfStatement(condition, codeBlock, elseStatement));
    }
}

Node::Pointer SyntaxParser::tryParseReturnStatement() {
    if(tryEat(TokenKind::keyword, Keywords::RETURN) == nullptr) {
        return nullptr;
    }
    
    auto expr = tryParseExpr();
    return std::static_pointer_cast<Node>(std::make_shared<ReturnStatement>(expr));
}

Pattern::Pointer SyntaxParser::tryParsePattern() {
    
    auto identifier = tryParseIdentifierExpr();
    if (identifier == nullptr) {
        return nullptr;
    }
    
    auto type = tryParseTypeAnnotation();
    return std::make_shared<Pattern>(identifier, type);
}

TypeDecl::Pointer SyntaxParser::tryParseTypeAnnotation() {
    if(tryEat(TokenKind::punctuation, Punctuations::COLON) == nullptr) {
        return nullptr;
    }
    
    TypeDecl::Pointer type = tryParseType();
    if(type == nullptr ) {
        return nullptr; // TODO: Report an error
    }
    return type;
}

TypeDecl::Pointer SyntaxParser::tryParseType() {
    auto identifier = tryParseIdentifierExpr();
    bool isOptinal = false;
    if(tryEat(TokenKind::operators, Operators::QUESTION) != nullptr) {
        isOptinal = true;
    }
    return std::make_shared<TypeDecl>(identifier, isOptinal);
}

// expression -> prefix-expression binary-expressions /opt/
Node::Pointer SyntaxParser::tryParseExpr() {
    std::shared_ptr<Node> expr = tryParsePrefixExpr();
    if(expr == nullptr) {
        return nullptr;
    }
    
    std::vector<Node::Pointer> binaries;
    
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
    std::shared_ptr<Token> assignmentOperator = tryEat(TokenKind::operators, Operators::EQULAS);
    if(assignmentOperator != nullptr) {
        std::shared_ptr<Node> prefixExpr = tryParseExpr();
        if(prefixExpr == nullptr) {
            // TODO: Error
            return nullptr;
        }

        return std::make_shared<AssignmentExpr>(prefixExpr);
    }
    
    auto binaryOperator = tryParseOperatorExpr();
    if(binaryOperator != nullptr) {
        std::shared_ptr<Node> prefixExpr = tryParsePrefixExpr();
        if(prefixExpr == nullptr) {
            // TODO: Error
            return nullptr;
        }
        return std::make_shared<BinaryExpr>(binaryOperator, prefixExpr);
    }

    return nullptr;
    
}

std::shared_ptr<Node> SyntaxParser::tryParsePrefixExpr() {
    auto op = tryParseOperatorExpr();
    
    std::shared_ptr<Node> postfixExpr = tryParsePostfixExpr();
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

Node::Pointer SyntaxParser::tryParsePostfixExpr() {
    std::shared_ptr<Node> result = nullptr;
    
    std::shared_ptr<Node> funcCallExpr = tryParseFuncCallExpr();
    if(funcCallExpr != nullptr) {
        auto postfixOperator = tryParsePostfixOperatorExpr();
        if(postfixOperator == nullptr) {
            result = funcCallExpr;
        } else {
            result = std::shared_ptr<Node>(new PostfixExpr(funcCallExpr, postfixOperator));
        }
    }
    
    Node::Pointer subscriptExpr = tryParseSubscriptExpr();
    if(subscriptExpr != nullptr) {
        auto postfixOperator = tryParsePostfixOperatorExpr();
        if(postfixOperator == nullptr) {
            result = subscriptExpr;
        } else {
            result = std::make_shared<PostfixExpr>(subscriptExpr, postfixOperator);
        }
    }
    
    if(result == nullptr) {
        std::shared_ptr<Node> primaryExpr = tryParsePrimaryExpr();
        if(primaryExpr != nullptr) {
            auto postfixOperator = tryParsePostfixOperatorExpr();
            if(postfixOperator == nullptr) {
                result = primaryExpr;
            } else {
                result = std::make_shared<PostfixExpr>(primaryExpr, postfixOperator);
            }
        }
    }
    
    if(result != nullptr) {
        if (tryEat(TokenKind::punctuation, Punctuations::DOT) != nullptr) {
            std::shared_ptr<Node> expr = tryParsePostfixExpr();
            if(expr == nullptr) {
                return nullptr; // TODO: Report a grammar;
            }
            result = std::shared_ptr<Node>(new MemberAccessExpr(result, expr));
        }
    }
    
    return result;
}

std::shared_ptr<Node> SyntaxParser::tryParseFuncCallExpr() {
    std::vector<std::shared_ptr<Token>>::const_iterator mark = iterator;
    auto identifier = tryParseIdentifierExpr();
    if(identifier == nullptr) {
        return nullptr;
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        iterator = mark;
        return nullptr;
    }
    
    std::vector<ArguCallExpr::Pointer> arguments;
    auto arguCall = tryParseArguCallExpr();
    if(arguCall != nullptr) {
        arguments.push_back(arguCall);
        while (true) {
            if(tryEat(TokenKind::punctuation, Punctuations::COMMA) == nullptr) {
                break;
            }
            
            auto arguCall = tryParseArguCallExpr();
            if(arguCall == nullptr) {
                Diagnostics::reportError(L"[Error]");
                return nullptr; //TODO: Report a grammar error
            }
            
            arguments.push_back(arguCall);
        }
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_ROUND_BRACKET) == nullptr) {
        return nullptr; // TODO: report an grammar error
    }
    
    return std::make_shared<FuncCallExpr>(identifier, arguments);
}

Node::Pointer SyntaxParser::tryParseSubscriptExpr() {
    std::vector<std::shared_ptr<Token>>::const_iterator mark = iterator;
    auto identifier = tryParseIdentifierExpr();
    if(identifier == nullptr) {
        return nullptr;
    }
    
    int loop = 0;
    
    std::vector<Node::Pointer> exprs;
    while (true) {
        if(tryEat(TokenKind::punctuation, Punctuations::OPEN_SQUARE_BRACKET) == nullptr) {
            if(loop == 0) {
                iterator = mark;
                return nullptr;
            }
            break;
        }
        
        auto expr = tryParseExpr();
        if(expr == nullptr) {
            Diagnostics::reportError(L"[Error] in array access");
            return nullptr;
        }
        exprs.push_back(expr);
        
        if(tryEat(punctuation, Punctuations::CLOSE_SQUARE_BRACKET) == nullptr) {
            Diagnostics::reportError(L"[Error] in array access");
            return nullptr;
        }
        
        loop ++;
    }
    
    if( loop == 0 ) {
        Diagnostics::reportError(L"[Error] in array access");
        return nullptr;
    }
    return std::make_shared<SubscriptExpr>(identifier, exprs);
    
}

ArguCallExpr::Pointer SyntaxParser::tryParseArguCallExpr() {
    auto identifier = tryParseIdentifierExpr();
    if(identifier == nullptr) {
        return nullptr;
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::COLON) == nullptr) {
        return nullptr; //TODO: report an grammar error
    }
    
    std::shared_ptr<Node> expr = tryParseExpr();
    if(expr == nullptr) {
        return nullptr; // TODO: report a grammar error
    }
    
    return std::make_shared<ArguCallExpr>(identifier, expr);
}


Node::Pointer SyntaxParser::tryParsePrimaryExpr() {
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

std::shared_ptr<Node> SyntaxParser::tryParseSelfExpr() {
    if(tryEat(TokenKind::keyword, Keywords::SELF) == nullptr) {
        return nullptr;
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::DOT) == nullptr) {
        return std::shared_ptr<Node>(new SelfExpr(nullptr));
    }
    
    auto identifier = tryParseIdentifierExpr();
    if(identifier == nullptr) {
        return nullptr; //TODO: Report n error
    }
    return std::shared_ptr<Node>(new SelfExpr(identifier));
}

Node::Pointer SyntaxParser::tryParseLiteralExpr() {
    auto literal = tryParseLiteral();
    if (literal != nullptr) {
        return std::make_shared<LiteralExpr>(literal);
    }
    
    auto arrayLiteral = tryParseArrayLiteralExpr();
    if(arrayLiteral != nullptr) {
        return arrayLiteral;
    }
    return nullptr;
}

Node::Pointer SyntaxParser::tryParseArrayLiteralExpr() {
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_SQUARE_BRACKET) == nullptr) {
        return nullptr;
    }
    
    std::vector<Node::Pointer> items;
    
    auto item = tryParseExpr();
    if(item !=  nullptr) {
        items.push_back(item);
        while (true) {
            if(tryEat(TokenKind::punctuation, Punctuations::COMMA) == nullptr) {
                break;
            }
            
            auto expr = tryParseExpr();
            if(expr == nullptr) {
                Diagnostics::reportError(L"[Error] Array literal except expression here");
                break;
            }
            
            items.push_back(expr);
        }
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_SQUARE_BRACKET) == nullptr) {
        Diagnostics::reportError(L"[Error] Array literal except expression here");
    }
    
    return std::make_shared<ArrayLiteralExpr>(items);
}

Node::Pointer SyntaxParser::tryParseParenthesizedExpr() {
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Node> expr = tryParseExpr();
    if(expr == nullptr) {
        Diagnostics::reportError(L"[Error] Parenthesized expr error 1");
        return nullptr;
    }

    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_ROUND_BRACKET) == nullptr) {
        Diagnostics::reportError(L"[Error] Parenthesized expr error 2");
        return nullptr; 
    }

    return std::make_shared<ParenthesizedExpr>(expr);
}

void SyntaxParser::tryParseConditionalOperator() {
}

OperatorExpr::Pointer SyntaxParser::tryParseOperatorExpr() {
    auto token = tryEat(TokenKind::operators);
    if(token == nullptr) {
        return nullptr;
    }
    return std::make_shared<OperatorExpr>(token);
}

OperatorExpr::Pointer SyntaxParser::tryParsePostfixOperatorExpr() {
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

IdentifierExpr::Pointer SyntaxParser::tryParseIdentifierExpr() {
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

Token::Pointer SyntaxParser::tryEat(TokenKind kind) {
    if (iterator != endIterator) {
        auto pToken = curToken();
        if (pToken->kind == kind) {
            iterator++;
            return pToken;
        }
    }

    return nullptr;
}

Token::Pointer SyntaxParser::tryEat(TokenKind kind, const std::wstring &value) {
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

std::shared_ptr<Token> SyntaxParser::curToken() const {
    return *iterator;
}
