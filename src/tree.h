#ifndef TREE_H
#define TREE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "node.h"

struct TreeRoot {
    Error status = UninitializedTree;
    size_t nodeCount  = 0;
    TreeNode* rootNode = NULL;
};

TreeRoot* attachRoot(TreeNode* node, Error* status = NULL);
Error treeInit(TreeRoot* root, TreeNode* node, NodeUnit data,
                    TreeNode* left = NULL, TreeNode* right = NULL);
TreeRoot*  treeDynamicInit(NodeUnit data,
                           TreeNode* left = NULL, TreeNode* right = NULL,
                           Error* status = NULL);

TreeRoot* treeRead(FILE* file, Error* status = NULL);

int treeTraverseInfix(TreeRoot* root,
                 int callbackFunction(TreeNode* node, void* data, uint level),
                 void* data = NULL, uint level = 0);

Error treePrintPrefix (FILE* file, TreeRoot* root);
Error treePrintInfix  (FILE* file, TreeRoot* root);
Error treePrintPostfix(FILE* file, TreeRoot* root);

Error treeDestroy(TreeRoot* tree, bool isAlloced = false);

int countNodesCallback(TreeNode* node, void* data, uint level);

#endif
