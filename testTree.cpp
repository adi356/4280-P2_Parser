#include "testTree.h"

/* Params: root node, node depth, prefix indentation, is node last */
void traversePreorder(NodeT *node, int depth, string indent, bool last) {
    if (node == NULL) {
        return;
    }
    string indentSymbol = "";
    if (depth > 0) {
        indentSymbol = (last) ? "└─ " : "├─ ";
    } else {
        indentSymbol = ">> ";
    }
    cout << indent << indentSymbol << node->label << " ";
    for (unsigned int i = 0; i < node->tokens.size(); i++) {
        cout << node->tokens[i].value;
        if(((i + 1) != node->tokens.size()) && (node->tokens[i].id != OP_tk)) {
            cout << ",";
        }
        cout << " ";
    }
    cout << endl;
    //indent levels
    indent += (last) ? "   " : "│  ";
    vector<NodeT *> nodes;
    if (node->c1 != NULL) {
        nodes.push_back(node->c1);
    }
    if (node->c2 != NULL) {
        nodes.push_back(node->c2);
    }
    if (node->c3 != NULL) {
        nodes.push_back(node->c3);
    }
    if (node->c4 != NULL) {
        nodes.push_back(node->c4);
    }
    //continue printing nodes
    for (unsigned int i = 0; i < nodes.size(); i++) {
        traversePreorder(nodes[i], depth + 1, indent, i == nodes.size() - 1);
    }
}