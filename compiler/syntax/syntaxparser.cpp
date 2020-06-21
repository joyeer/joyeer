#include "syntaxparser.h"
#include "compiler/AST/ast.h"

SyntaxParser::SyntaxParser(const std::vector<std::shared_ptr<Token>> &tokens) : tokens(tokens)
{
    iterator = tokens.begin();
    endIterator = tokens.end();
}

SourceBlock::Pointer SyntaxParser::parse() {
    std::vector<std::shared_ptr<Node>> decls;
    while(iterator != endIterator) {
        std::shared_ptr<Node> decl = tryParseDecl();
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

std::shared_ptr<Node> SyntaxParser::tryParseDecl() {
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
    
    std::shared_ptr<Node> initializer = tryParseInitializerDecl();
    if(initializer != nullptr) {
        return initializer;
    }

    return nullptr;
}

std::shared_ptr<Node> SyntaxParser::tryParseFunctionDecl() {
    if(tryEat(TokenKind::keyword, Keywords::FUNC) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Token> identifier = tryParseIdentifier();
    if(identifier == nullptr) {
        return nullptr; //TODO: report a grammar error here.
    }

    std::shared_ptr<Node> parameterClause = tryParseParameterClause();
    if(parameterClause == nullptr) {
        return nullptr; // TODO: report an grammar error here.
    }

    std::shared_ptr<Node> codeBlock = tryParseCodeBlock();
    if(codeBlock == nullptr) {
        return nullptr; // TODO: Error
    }
    
    return std::shared_ptr<Node>(new FuncDecl(identifier, parameterClause, codeBlock));
}

std::shared_ptr<Node> SyntaxParser::tryParseInitializerDecl() {
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

    return std::shared_ptr<Node>(new InitializerDecl(parameterClause, codeBlock));
}

std::shared_ptr<Node> SyntaxParser::tryParseParameterClause() {
    if (tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        return nullptr;
    }
    // parse the parameters declaration in function
    std::vector<std::shared_ptr<Node>> parameters;
    int i = 0 ;
    while(true) {
        std::shared_ptr<Token> comma = nullptr;
        if( i > 0 ) {
            comma = tryEat(TokenKind::punctuation, Punctuations::COMMA);
        }
        
        std::shared_ptr<Node> identifier = tryParsePattern();
        
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

    std::shared_ptr<Node> pattern = tryParsePattern();
    if (pattern == nullptr) {
        return nullptr; //TODO: report an syntax Error
    }
    
    std::shared_ptr<Node> expr = tryParseExpr();
    return std::shared_ptr<Node>(new ConstDecl(pattern, expr));
}

std::shared_ptr<Node> SyntaxParser::tryParseVarDecl() {
    if(tryEat(TokenKind::keyword, Keywords::VAR) == nullptr) {
        return nullptr; // Not a 'var' declaration
    }

    std::shared_ptr<Node> pattern = tryParsePattern();
    if (pattern == nullptr) {
       return nullptr; //TODO: report an syntax Error
    }

    std::shared_ptr<Node> expr = tryParseExpr();
    return std::shared_ptr<Node>(new VarDecl(pattern, expr));
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

std::shared_ptr<Node> SyntaxParser::tryParseStatement() {
    std::shared_ptr<Node> expr = tryParseExpr();
    if(expr != nullptr) {
        return expr;
    }

    std::shared_ptr<Node> decl = tryParseDecl();
    if(decl != nullptr) {
        return decl;
    }

    std::shared_ptr<Node> loopStatement = tryParseLoopStatement();
    if(loopStatement != nullptr) {
        return loopStatement;
    }
    
    std::shared_ptr<Node> branchStatement = tryParseBranchStatement();
    if(branchStatement != nullptr) {
        return branchStatement;
    }

    return loopStatement;
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

std::shared_ptr<Node> SyntaxParser::tryParsePattern() {
    
    std::shared_ptr<Token> identifier = tryParseIdentifier();
    if (identifier == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Node> type = tryParseTypeAnnotation();
    return std::shared_ptr<Node>(new Pattern(identifier, type));
}

std::shared_ptr<Node> SyntaxParser::tryParseTypeAnnotation() {
    if(tryEat(TokenKind::punctuation, Punctuations::COLON) == nullptr) {
        return nullptr;
    }
    
    std::shared_ptr<Node> type = tryParseType();
    if(type == nullptr ) {
        return nullptr; // TODO: Report an error
    }
    return type;
}

std::shared_ptr<Node> SyntaxParser::tryParseType() {
    std::shared_ptr<Token> identifier = tryParseIdentifier();
    bool isOptinal = false;
    if(tryEat(TokenKind::operators, Operators::QUESTION) != nullptr) {
        isOptinal = true;
    }
    return std::shared_ptr<Node>(new Type(identifier, isOptinal));
}

// expression -> prefix-expression /opt/ binary-expressions /opt/
std::shared_ptr<Node> SyntaxParser::tryParseExpr() {
    std::shared_ptr<Node> expr = tryParsePrefixExpr();
    if(expr == nullptr) {
        return nullptr;
    }
    
    std::shared_ptr<Node> binExpr = tryParseBinaryExpr();
    
    return std::shared_ptr<Node>(new Expr(expr, binExpr));
}

std::shared_ptr<Node> SyntaxParser::tryParsePrefixExpr() {
    std::shared_ptr<Token> prefixOperator = tryEat(TokenKind::operators);
    
    std::shared_ptr<Node> postfixExpr = tryParsePostfixExpr();
    if(postfixExpr == nullptr) {
        // revert the prefix operator
        if(prefixOperator != nullptr) {
            previous();
        }
        return nullptr;
    }

    if(prefixOperator == nullptr) {
        return postfixExpr;
    } else {
        return std::shared_ptr<PrefixExpr>(new PrefixExpr(prefixOperator, postfixExpr));
    }
}

std::shared_ptr<Node> SyntaxParser::tryParsePostfixExpr() {
    std::shared_ptr<Node> result = nullptr;
    
    std::shared_ptr<Node> funcCallExpr = tryParseFunctionCallExpr();
    if(funcCallExpr != nullptr) {
        result = std::shared_ptr<Node>(new PostfixExpr(funcCallExpr, tryParsePostfixOperator()));
    }
    
    if(result == nullptr) {
        std::shared_ptr<Node> primaryExpr = tryParsePrimaryExpr();
        if(primaryExpr != nullptr) {
            result = std::shared_ptr<PostfixExpr>(new PostfixExpr(primaryExpr, tryParsePostfixOperator()));
        }
    }
    
    if(result != nullptr) {
        if (tryEat(TokenKind::punctuation, Punctuations::DOT) != nullptr) {
            std::shared_ptr<Node> expr = tryParsePostfixExpr();
            if(expr == nullptr) {
                return nullptr; // TODO: Report a grammar;
            }
            std::shared_ptr<Node> memberExpr = std::shared_ptr<Node>(new MemberExpr(result, expr));
        }
    }
    
    return result;
}

std::shared_ptr<Node> SyntaxParser::tryParseFunctionCallExpr() {
    std::vector<std::shared_ptr<Token>>::const_iterator mark = iterator;
    std::shared_ptr<Token> identifier = tryParseIdentifier();
    if(identifier == nullptr) {
        return nullptr;
    }
    
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        iterator = mark;
        return nullptr;
    }
    
    std::vector<std::shared_ptr<Node>> arguments;
    std::shared_ptr<Node> arguCall = tryParseArguCallExpr();
    if(arguCall != nullptr) {
        arguments.push_back(arguCall);
        while (true) {
            if(tryEat(TokenKind::punctuation, Punctuations::COMMA) == nullptr) {
                break;
            }
            
            std::shared_ptr<Node> arguCall = tryParseArguCallExpr();
            if(arguCall == nullptr) {
                return nullptr; //TODO: Report a grammar error
            }
            
            arguments.push_back(arguCall);
        }
    }
    
    
    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_ROUND_BRACKET) == nullptr) {
        return nullptr; // TODO: report an grammar error
    }
    
    
    return std::shared_ptr<Node>(new FunctionCallExpr(identifier, arguments));
}

std::shared_ptr<Node> SyntaxParser::tryParseArguCallExpr() {
    std::shared_ptr<Token> identifier = tryParseIdentifier();
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
    
    return std::shared_ptr<Node>(new ArguCallExpr(identifier, expr));
}

std::shared_ptr<Node> SyntaxParser::tryParseBinaryExpr() {
    std::shared_ptr<Token> assignmentOperator = tryEat(TokenKind::operators, Operators::EQULAS);
    if(assignmentOperator != nullptr) {
        std::shared_ptr<Node> prefixExpr = tryParsePrefixExpr();
        if(prefixExpr == nullptr) {
            // TODO: Error
            return nullptr;
        }

        return std::shared_ptr<Node>(new AssignmentExpr(prefixExpr));
    }
    
    std::shared_ptr<Token> binaryOperator = tryEat(TokenKind::operators);
    if(binaryOperator != nullptr) {
        std::shared_ptr<Node> prefixExpr = tryParsePrefixExpr();
        if(prefixExpr == nullptr) {
            // TODO: Error
            return nullptr;
        }
        return  std::shared_ptr<Node>(new BinaryExpr(binaryOperator, prefixExpr));
    }

    return nullptr;
    
}

std::shared_ptr<Node> SyntaxParser::tryParsePrimaryExpr() {
    std::shared_ptr<Token> identifier = tryParseIdentifier();
    if (identifier != nullptr) {
        return std::shared_ptr<Node>(new IdentifierExpr(identifier));
    }

    std::shared_ptr<Node> literalExpr = tryParseLiteralExpr();
    if (literalExpr != nullptr) {
        return literalExpr;
    }
    
    std::shared_ptr<Node> selfExpr = tryParseSelfExpr();
    if(selfExpr != nullptr) {
        return selfExpr;
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
    
    std::shared_ptr<Token> identifier = tryParseIdentifier();
    if(identifier == nullptr) {
        return nullptr; //TODO: Report n error
    }
    return std::shared_ptr<Node>(new SelfExpr(identifier));
}

std::shared_ptr<LiteralExpr> SyntaxParser::tryParseLiteralExpr() {
    std::shared_ptr<Token> literal = tryParseLiteral();
    if (literal != nullptr) {
        return std::shared_ptr<LiteralExpr>(new LiteralExpr(literal));
    }
    return nullptr;
}

std::shared_ptr<Node> SyntaxParser::tryParseParenthesizedExpr() {
    if(tryEat(TokenKind::punctuation, Punctuations::OPEN_ROUND_BRACKET) == nullptr) {
        return nullptr;
    }

    std::shared_ptr<Node> expr = tryParseExpr();
    if(expr == nullptr) {
        return nullptr;
        // TODO: error
    }

    if(tryEat(TokenKind::punctuation, Punctuations::CLOSE_ROUND_BRACKET) == nullptr) {
        return nullptr; //TODO: Error
    }

    return std::shared_ptr<Node>(new ParenthesizedExpr(expr));
}

void SyntaxParser::tryParseConditionalOperator() {
}

std::shared_ptr<Token> SyntaxParser::tryParseOperator() {
    return tryEat(TokenKind::operators);
}

std::shared_ptr<Token> SyntaxParser::tryParsePostfixOperator() {
    std::shared_ptr<Token> token = tryEat(TokenKind::operators, Operators::QUESTION);
    if(token != nullptr) {
        return token;
    }
    
    std::shared_ptr<Token> pointToken = tryEat(TokenKind::operators, Operators::POINT);
    if(pointToken != nullptr) {
        return pointToken;
    }
    
    return nullptr;
}

std::shared_ptr<Token> SyntaxParser::tryParseIdentifier() {
    return tryEat(TokenKind::identifier);
}

std::shared_ptr<Token> SyntaxParser::tryParseLiteral()
{
    std::shared_ptr<Token> literal = tryEat(TokenKind::stringLiteral);
    if (literal != nullptr)
    {
        return literal;
    }

    literal = tryEat(TokenKind::decimalLiteral);
    if (literal != nullptr)
    {
        return literal;
    }

    literal = tryEat(TokenKind::floatLiteral);
    if (literal != nullptr)
    {
        return literal;
    }

    literal = tryEat(TokenKind::nilLiteral);
    if (literal != nullptr)
    {
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

std::shared_ptr<Token> SyntaxParser::tryEat(TokenKind kind) {
    if (iterator != endIterator) {
        std::shared_ptr<Token> pToken = curToken();
        if (pToken->kind == kind) {
            iterator++;
            return pToken;
        }
    }

    return nullptr;
}

std::shared_ptr<Token> SyntaxParser::tryEat(TokenKind kind, const std::wstring &value) {
    std::shared_ptr<Token> pToken = tryEat(kind);
    if(pToken == nullptr) {
        return nullptr;
    }
    
    if (pToken->value == value) {
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
