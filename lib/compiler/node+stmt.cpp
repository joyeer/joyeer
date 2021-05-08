#include "joyeer/compiler/node+stmt.h"


FileImportStmt::FileImportStmt(Token::Ptr stringLiteral):
Node(SyntaxKind::fileimportStmt),
stringLiteral(stringLiteral) {
}

const std::string FileImportStmt::getImportedFilename() {
    return stringLiteral->rawValue;
}

ForInStmt::ForInStmt(Node::Ptr pattern, Node::Ptr inExpr, Node::Ptr codeBlock):
Node(SyntaxKind::forInStmt),
pattern(pattern),
inExpr(inExpr),
codeBlock(codeBlock) {
}

WhileStmt::WhileStmt(Node::Ptr expr, Node::Ptr codeBlock):
Node(SyntaxKind::whileStmt),
expr(expr),
codeBlock(codeBlock) {
}

IfStmt::IfStmt(Node::Ptr condition, Node::Ptr ifCodeBlock, std::shared_ptr<Node> elseCodeBlock):
Node(SyntaxKind::ifStmt),
condition(condition),
ifCodeBlock(ifCodeBlock),
elseCodeBlock(elseCodeBlock) {
}

ReturnStmt::ReturnStmt(Node::Ptr expr):
Node(SyntaxKind::returnStmt),
expr(expr) {
}
