#ifndef DERIVATIVE_H
#define DERIVATIVE_H

#include "ds/tree/node.h"
#include "diff/io/io.h"
 
TreeNode* differentiate(Context* context, TreeNode* node, const char* var);

#endif
