#ifndef NODE_H
#define NODE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "nodetype.h"
#include "../../misc/error.h"

union NodeValue {
    OpType op;
    char var;
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

enum TreeStatus {
    UninitializedTree = NON_GENERIC_ERROR,
    DestroyedTree,
    AttemptedReinitialization,
    FailReadNode,
};

Error nodeInit(TreeNode* node, NodeUnit data, TreeNode* parent = NULL,
               TreeNode* left = NULL, TreeNode* right = NULL);
TreeNode*  nodeDynamicInit(NodeUnit data, TreeNode* parent = NULL,
                           TreeNode* left = NULL, TreeNode* right = NULL,
                           Error* status = NULL);

TreeNode* nodeRead(FILE* file, Error* status = NULL, size_t* nodeCount = NULL);

/// Universal infix traverse - stops if callbackFunction returns true
typedef Error (*callback_f)(TreeNode* node, void* data, uint level);

Error nodeTraverseInfix(TreeNode* node,
                        callback_f callbackFunction,
                        void* data = NULL, uint level = 0);
Error nodeTraversePrefix (TreeNode* node,
                          callback_f callbackFunction,
                          void* data = NULL, uint level = 0);

Error countNodesCallback(TreeNode* node, void* data, uint level);
Error findVariableCallback(TreeNode* node, void* data, uint level);
// copy-paste postfix traverses...

Error nodePrintPrefix (FILE* file, TreeNode* node);
Error nodePrintInfix  (FILE* file, TreeNode* node);
Error nodePrintPostfix(FILE* file, TreeNode* node);

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
