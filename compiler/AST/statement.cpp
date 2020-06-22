#include "ast.h"

SourceBlock::SourceBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::sourceBlock),
statements(statements) {

}

CodeBlock::CodeBlock(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::codeBlock),
statements(statements) {
}

ForInStatement::ForInStatement(std::shared_ptr<Node> pattern, std::shared_ptr<Node> inExpr, std::shared_ptr<Node> codeBlock):
Node(SyntaxKind::forInStatement),
pattern(pattern),
inExpr(inExpr),
codeBlock(codeBlock) {
}

IfStatement::IfStatement(std::shared_ptr<Node> condition, std::shared_ptr<Node> ifCodeBlock, std::shared_ptr<Node> elseCodeBlock): 
Node(SyntaxKind::ifStatement), 
condition(condition),
ifCodeBlock(ifCodeBlock),
elseCodeBlock(elseCodeBlock) {

}
