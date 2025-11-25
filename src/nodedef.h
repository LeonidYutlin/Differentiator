#ifndef NODE_DEF_H
#define NODE_DEF_H

#include "node.h"
#include "math.h"

struct TreeNode{
    NodeType  type   = NUM_TYPE;
    NodeUnit  data   = NAN;
    TreeNode* parent = NULL;
    TreeNode* left   = NULL;
    TreeNode* right  = NULL;
};

#endif
