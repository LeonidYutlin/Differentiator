#ifndef NODE_DEF_H
#define NODE_DEF_H

#include "node.h"
#include "nodetype.h"
#include "math.h"

struct TreeNode{
    NodeUnit  data   = {};
    TreeNode* parent = NULL;
    TreeNode* left   = NULL;
    TreeNode* right  = NULL;
};

#endif
