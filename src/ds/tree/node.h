#ifndef NODE_H
#define NODE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "ds/tree/nodetype.h"
#include "error/error.h"

union NodeValue {
  OpType op;
  size_t var; //var_id actually
  double num;
};

struct NodeUnit {
  NodeType type = UNKNOWN_TYPE;
  NodeValue value;
};

struct TreeNode {
  NodeUnit  data   = {};
  TreeNode* parent = NULL;
  TreeNode* left   = NULL;
  TreeNode* right  = NULL;
};

Error nodeInit(TreeNode* node, NodeUnit data, TreeNode* parent = NULL,
               TreeNode* left = NULL, TreeNode* right = NULL);
TreeNode* nodeAlloc(NodeUnit data, TreeNode* parent = NULL,
                    TreeNode* left = NULL, TreeNode* right = NULL,
                    Error* status = NULL);

typedef Error (*callback_f)(TreeNode* node, void* data, uint level);

struct NodeTraverseOpt {
  callback_f prefix  = NULL;
  callback_f infix   = NULL;
  callback_f postfix = NULL;
  void* prefixData   = NULL;
  void* infixData    = NULL;
  void* postfixData  = NULL;
  uint level = 0;
};

/// Universal Traverse - stops if any callback_f return non-zero
Error nodeTraverse(TreeNode* node, NodeTraverseOpt opt);

#define nodeTraverse(node, ...) \
  nodeTraverse(node, (NodeTraverseOpt){ __VA_ARGS__ })

Error countNodesCallback(TreeNode* node, void* data, uint level);
Error findVariableCallback(TreeNode* node, void* data, uint level);

//Takes parent, seaches for child as its child, replaces that child with newChild and frees child
//Note: if your newChild and child are in a relationship then you should make sure they aren't by
//breaking the bound before calling the function
Error nodeChangeChild(TreeNode* parent, TreeNode* child, TreeNode* newChild,
                      size_t* nodeCount);

///Note: doesnt copy the parent field but instead assigns newParent as copy's parent
TreeNode*  nodeCopy(TreeNode* srcNode, TreeNode* newParent, Error* status = NULL);
void nodeFixParents(TreeNode* node);
Error nodeOptimize(TreeNode** node);

Error  nodeDelete(TreeNode* node, bool isAlloced = false, size_t* nodeCount = NULL);
Error nodeDestroy(TreeNode* node, bool isAlloced = false, size_t* nodeCount = NULL);

#endif
