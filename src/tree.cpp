#include "treedef.h"
#include <stdlib.h>

#define RETURN_WITH_STATUS(value, returnValue) \
        { \
        if (status) \
            *status = value; \
        return returnValue; \
        }

TreeStatus treeInit(TreeRoot* root, TreeNode* node, NodeType type, NodeUnit data,
                    TreeNode* left, TreeNode* right) {
    if (!root || !node)
        return InvalidParameters;
    if (root->status != UninitializedTree &&
        root->status != DestroyedTree)
        return AttemptedReinitialization;

    TreeStatus status = nodeInit(node, type, data, NULL, left, right);
    if (status)
        return status;
    root->rootNode = node;
    root->nodeCount = 1;
    root->status = OK;

    return OK; //return treeVerify(root);
}

TreeRoot* treeDynamicInit(NodeType type, NodeUnit data,
                          TreeNode* left, TreeNode* right,
                          TreeStatus* status) {
    TreeRoot* root = (TreeRoot*)calloc(1, sizeof(TreeRoot));
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!root || !node) {
        free(root); free(node);
        RETURN_WITH_STATUS(FailMemoryAllocation, NULL);
    }
    root->status = UninitializedTree;

    TreeStatus returnedStatus = treeInit(root, node, type, data, left, right);
    if (returnedStatus) {
        free(root); free(node);
        RETURN_WITH_STATUS(returnedStatus, NULL);
    }

    return root; //return treeVerify(root);
}

int treeTraverse(TreeRoot* root,
                 int cb(TreeNode* node, void* data, uint level),
                 void* data, uint level) {
	if (!root)
        return OK;

	return nodeTraverse(root->rootNode, cb, data, level);
}

TreeStatus treePrintPrefix(FILE* f, TreeRoot* root) {
    if (!root)
        return InvalidParameters;

    return nodePrintPrefix(f, root->rootNode);
}

TreeStatus treePrintInfix(FILE* f, TreeRoot* root) {
    if (!root)
        return InvalidParameters;

    return nodePrintInfix(f, root->rootNode);
}

TreeStatus treePrintPostfix(FILE* f, TreeRoot* root) {
    if (!root)
        return InvalidParameters;

    return nodePrintPostfix(f, root->rootNode);
}

TreeStatus treeDestroy(TreeRoot* root, bool isAlloced) {
    if (!root)
        return InvalidParameters;

    nodeDestroy(root->rootNode, isAlloced, NULL);
    root->status = DestroyedTree;

    if (isAlloced)
        free(root);

    return OK;
}

#undef RETURN_WITH_STATUS
