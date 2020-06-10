#include "decl.h"

AssignmentOperator::AssignmentOperator():
  Node(SyntaxKind::assignmentOperator) {

}

BinaryOperator::BinaryOperator(std::shared_ptr<Token> o):
  Node(SyntaxKind::binaryOperator),
  op(o) {

}