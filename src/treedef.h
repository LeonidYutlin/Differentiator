#ifndef TREE_DEF_H
#define TREE_DEF_H

#include "tree.h"
#include "nodedef.h"

struct TreeRoot{
    TreeStatus status = UninitializedTree;
    size_t nodeCount  = 0;
    TreeNode* rootNode = NULL;
};

#endif
