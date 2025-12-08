#include "treedef.h"
#include <stdlib.h>

#define RETURN_WITH_STATUS(value, returnValue) \
        { \
        if (status) \
            *status = value; \
        return returnValue; \
        }

Error treeInit(TreeRoot* root, TreeNode* node, NodeUnit data,
                    TreeNode* left, TreeNode* right) {
    if (!root || !node)
        return InvalidParameters;
    if (root->status != UninitializedTree &&
        root->status != DestroyedTree)
        return AttemptedReinitialization;

    Error status = nodeInit(node, data, NULL, left, right);
    if (status)
        return status;
    root->rootNode = node;
    root->nodeCount = 1;
    root->status = OK;

    return OK; //return treeVerify(root);
}

TreeRoot* treeDynamicInit(NodeUnit data,
                          TreeNode* left, TreeNode* right,
                          Error* status) {
    TreeRoot* root = (TreeRoot*)calloc(1, sizeof(TreeRoot));
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!root || !node) {
        free(root); free(node);
        RETURN_WITH_STATUS(FailMemoryAllocation, NULL);
    }
    root->status = UninitializedTree;

    Error returnedStatus = treeInit(root, node, data, left, right);
    if (returnedStatus) {
        free(root); free(node);
        RETURN_WITH_STATUS(returnedStatus, NULL);
    }

    return root; //return treeVerify(root);
}

TreeRoot* treeRead(FILE* file, Error* status) {
    if (!file)
        RETURN_WITH_STATUS(InvalidParameters, NULL);

    Error returnedStatus = OK;
    TreeRoot* root = treeDynamicInit((NodeUnit){}, NULL, NULL, &returnedStatus);
    if (returnedStatus)
        RETURN_WITH_STATUS(returnedStatus, NULL);

    root->nodeCount = 0;
    TreeNode* node = nodeRead(file, &returnedStatus, &root->nodeCount);
    if (returnedStatus) {
        treeDestroy(root, true);
        RETURN_WITH_STATUS(returnedStatus, NULL);
    }

    nodeDestroy(root->rootNode, true);
    root->rootNode = node;
    return root;
}

int treeTraverse(TreeRoot* root,
                 int cb(TreeNode* node, void* data, uint level),
                 void* data, uint level) {
	if (!root)
        return OK;

	return nodeTraverse(root->rootNode, cb, data, level);
}

Error treePrintPrefix(FILE* f, TreeRoot* root) {
    if (!root)
        return InvalidParameters;

    return nodePrintPrefix(f, root->rootNode);
}

Error treePrintInfix(FILE* f, TreeRoot* root) {
    if (!root)
        return InvalidParameters;

    return nodePrintInfix(f, root->rootNode);
}

Error treePrintPostfix(FILE* f, TreeRoot* root) {
    if (!root)
        return InvalidParameters;

    return nodePrintPostfix(f, root->rootNode);
}

Error treeDestroy(TreeRoot* root, bool isAlloced) {
    if (!root)
        return InvalidParameters;

    nodeDestroy(root->rootNode, isAlloced, NULL);
    root->status = DestroyedTree;

    if (isAlloced)
        free(root);

    return OK;
}

#undef RETURN_WITH_STATUS
