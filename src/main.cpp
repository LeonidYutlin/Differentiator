#include "treedef.h"
#include "nodedef.h"
#include "dump.h"

int main() {
    FILE* log = initLogFile();

    FILE* expr = fopen(".test/expr.txt", "r");
    TreeNode* node = nodeRead(expr);
    if (!node)
        fprintf(stderr, "IT IS NULL\n");
    fclose(expr);
    nodeDump(log, node, "<b3>Read tree</b3>");

    TreeStatus status = OK;
    TreeRoot* root = treeDynamicInit(OP_TYPE, OP_MULTIPLY, NULL, NULL, &status);
    TreeNode* x    = nodeDynamicInit(VAR_TYPE, 'x', root->rootNode);
    root->rootNode->left = x;
    TreeNode* add  = nodeDynamicInit(OP_TYPE, OP_PLUS, root->rootNode);
    root->rootNode->right = add;
    TreeNode* y    = nodeDynamicInit(VAR_TYPE, 'y', add);
    add->left = y;
    TreeNode* num6 = nodeDynamicInit(NUM_TYPE, 6, add);
    add->right = num6;

    treeDump(log, root, "<b3>Manually made tree</b3>");

//     TreeNode* num7 = nodeDynamicInit(NUM_TYPE, 7, num6);
//     num6->right = num7;
//     TreeNode* num8 = nodeDynamicInit(NUM_TYPE, 8, x);
//     x->left = num8;
//
//     treeDump(log, root, "<b3>Manually made tree</b3>");

    treeDestroy(root, true);
    nodeDestroy(node, true);
}

