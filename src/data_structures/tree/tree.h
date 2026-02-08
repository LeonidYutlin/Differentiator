#ifndef TREE_H
#define TREE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "data_structures/tree/node.h"

struct TreeRoot {
  Error status = UninitializedTree;
  size_t nodeCount  = 0;
  TreeNode* rootNode = NULL;
};

TreeRoot* attachRoot(TreeNode* node, Error* status = NULL);
TreeNode* detachRoot(TreeRoot* root, Error* status = NULL);
Error treeInit(TreeRoot* root, TreeNode* node, NodeUnit data,
                    TreeNode* left = NULL, TreeNode* right = NULL);
TreeRoot* treeAlloc(NodeUnit data,
                    TreeNode* left = NULL, TreeNode* right = NULL,
                    Error* status = NULL);

#define treeTraverse(root, ...) \
  nodeTraverse((root)->rootNode, (NodeTraverseOpt){ __VA_ARGS__ })

Error treeDestroy(TreeRoot* tree, bool isAlloced = false);

#endif
