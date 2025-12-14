#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include <stddef.h>
#include <sys/types.h>
#include "../../misc/util.h"

const size_t MAX_VALUE_STRING_LENGTH = 16;

enum NodeType {
    UNKNOWN_TYPE,
    OP_TYPE,
    NUM_TYPE,
    VAR_TYPE,
};

enum OpType {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_SIN,
    OP_COS,
    OP_TAN,
    OP_COT,
};

#define IS_OP(node) \
    (node && \
     node->data.type == OP_TYPE)

#define IS_NUM(node) \
    (node && \
     node->data.type == NUM_TYPE)

#define OP_OF(node, opType) \
    (IS_OP(node) && \
     (OpType)node->data.value == opType)

#define NUM_OF(node, i) \
    (IS_NUM(node) && \
     doubleEqual(node->data.value, i))

const char* getNodeTypeString(NodeType type);
const char* getOpTypeString(OpType type);
int getOpType(const char* string);
uint getOpTypeArgumentCount(OpType type);
uint getOpTypePriority(OpType type);

#endif
