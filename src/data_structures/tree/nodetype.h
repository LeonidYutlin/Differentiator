#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include <stddef.h>
#include <sys/types.h>

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
};

const char* getNodeTypeString(NodeType type);
const char* getOpTypeString(OpType type);
int getOpType(const char* string);
uint getOpTypeArgumentCount(OpType type);
uint getOpTypePriority(OpType type);

#endif
