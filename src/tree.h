#ifndef TREE_H
#define TREE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "node.h"

struct TreeRoot;

TreeStatus treeInit(TreeRoot* root, TreeNode* node, NodeType type, NodeUnit data,
                    TreeNode* left = NULL, TreeNode* right = NULL);
TreeRoot*  treeDynamicInit(NodeType type, NodeUnit data,
                           TreeNode* left = NULL, TreeNode* right = NULL,
                           TreeStatus* status = NULL);

TreeRoot* treeRead(FILE* file, TreeStatus* status = NULL);

int treeTraverse(TreeRoot* root,
                 int callbackFunction(TreeNode* node, void* data, uint level),
                 void* data = NULL, uint level = 0);

TreeStatus treePrintPrefix (FILE* file, TreeRoot* root);
TreeStatus treePrintInfix  (FILE* file, TreeRoot* root);
TreeStatus treePrintPostfix(FILE* file, TreeRoot* root);

TreeStatus treeDestroy(TreeRoot* tree, bool isAlloced = false);

#endif
