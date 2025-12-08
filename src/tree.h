#ifndef TREE_H
#define TREE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "node.h"

struct TreeRoot;

Error treeInit(TreeRoot* root, TreeNode* node, NodeUnit data,
                    TreeNode* left = NULL, TreeNode* right = NULL);
TreeRoot*  treeDynamicInit(NodeUnit data,
                           TreeNode* left = NULL, TreeNode* right = NULL,
                           Error* status = NULL);

TreeRoot* treeRead(FILE* file, Error* status = NULL);

int treeTraverse(TreeRoot* root,
                 int callbackFunction(TreeNode* node, void* data, uint level),
                 void* data = NULL, uint level = 0);

Error treePrintPrefix (FILE* file, TreeRoot* root);
Error treePrintInfix  (FILE* file, TreeRoot* root);
Error treePrintPostfix(FILE* file, TreeRoot* root);

Error treeDestroy(TreeRoot* tree, bool isAlloced = false);

#endif
