#ifndef NODE_H
#define NODE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

const size_t MAX_VALUE_STRING_LENGTH = 16;

typedef double NodeUnit;
struct TreeNode;

enum TreeStatus {
    OK = 0,
    UninitializedTree,
    DestroyedTree,
    AttemptedReinitialization,
    InvalidParameters,
    FailMemoryAllocation,
    FailReadNode,
};

enum NodeType {
    OP_TYPE,
    NUM_TYPE,
    VAR_TYPE,
};

enum OpType {
    OP_PLUS,
    OP_MINUS,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_POWER,
};

TreeStatus nodeInit(TreeNode* node, NodeType type, NodeUnit data, TreeNode* parent = NULL,
                    TreeNode* left = NULL, TreeNode* right = NULL);
TreeNode*  nodeDynamicInit(NodeType type, NodeUnit data, TreeNode* parent = NULL,
                           TreeNode* left = NULL, TreeNode* right = NULL,
                           TreeStatus* status = NULL);

TreeNode* nodeRead(FILE* file, TreeStatus* status = NULL, size_t* nodeCount = NULL);

/// Universal infix traverse - stops if callbackFunction returns true
int nodeTraverse(TreeNode* node,
                 int callbackFunction(TreeNode* node, void* data, uint level),
                 void* data = NULL, uint level = 0);
// copy-paste postfix and prefix traverses...

TreeStatus nodePrintPrefix (FILE* file, TreeNode* node);
TreeStatus nodePrintInfix  (FILE* file, TreeNode* node);
TreeStatus nodePrintPostfix(FILE* file, TreeNode* node);

TreeStatus nodeDelete(TreeNode* node, bool isAlloced = false, size_t* nodeCount = NULL);

TreeStatus nodeDestroy(TreeNode* node, bool isAlloced = false, size_t* nodeCount = NULL);

const char* getNodeTypeString(NodeType type);
const char* getOpTypeString(OpType type);
int getOpType(const char* string);
uint getOpTypeArgumentCount(OpType type);
uint getOpTypePriority(OpType type);

#endif
