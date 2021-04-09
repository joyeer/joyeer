#include "joyeer/compiler/node+types.h"

FileModuleNode::FileModuleNode(std::vector<std::shared_ptr<Node>> statements):
Node(SyntaxKind::sourceBlock),
statements(statements) {

}

std::string FileModuleNode::getName() {
    std::filesystem::path p = filename;
    return p.replace_extension().string();
}

std::vector<FileImportDecl::Ptr> FileModuleNode::getFileImports() {
    std::vector<FileImportDecl::Ptr> result;
    for(std::vector<Node::Ptr>::const_iterator iterator = statements.begin(); iterator != statements.end(); iterator ++ ) {
        auto node = *iterator;
        if(node->kind == fileimportDecl) {
            result.push_back(std::static_pointer_cast<FileImportDecl>(node));
        }
    }
    return result;
}