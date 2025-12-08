#ifndef NODE_H
#define NODE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "nodetype.h"
#include "error.h"

typedef struct NodeUnit{
    NodeType type = UNKNOWN_TYPE;
    double value = NAN;
} NodeUnit;
struct TreeNode;

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
int nodeTraverseInfix(TreeNode* node,
                 int callbackFunction(TreeNode* node, void* data, uint level),
                 void* data = NULL, uint level = 0);
int nodeTraversePrefix (TreeNode* node,
                        int callbackFunction(TreeNode* node, void* data, uint level),
                        void* data = NULL, uint level = 0);
// copy-paste postfix traverses...

Error nodePrintPrefix (FILE* file, TreeNode* node);
Error nodePrintInfix  (FILE* file, TreeNode* node);
Error nodePrintPostfix(FILE* file, TreeNode* node);

///Note: doesnt copy the parent field but instead assigns newParent as copy's parent
TreeNode*  nodeCopy(TreeNode* srcNode, TreeNode* newParent, Error* status = NULL);
void nodeFixParents(TreeNode* node);

Error  nodeDelete(TreeNode* node, bool isAlloced = false, size_t* nodeCount = NULL);
Error nodeDestroy(TreeNode* node, bool isAlloced = false, size_t* nodeCount = NULL);

#endif
