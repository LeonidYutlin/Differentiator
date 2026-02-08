#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include "data_structures/tree/node.h"
#include "diff/io.h"
 
TreeNode* differentiate(Context* context, TreeNode* node, const char* var);

#endif
