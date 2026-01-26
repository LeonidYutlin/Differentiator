#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include <stddef.h>
#include <sys/types.h>
#include <math.h>
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
    OP_LOG,
    OP_LN,
    OP_ASIN,
    OP_ACOS,
    OP_ATAN,
    OP_ACOT,
    OP_SINH,
    OP_COSH,
    OP_TANH,
    OP_COTH,
};

#define IS_OP(node)                \
    (node &&                       \
     (node)->data.type == OP_TYPE)

#define IS_NUM(node)                \
    (node &&                        \
     (node)->data.type == NUM_TYPE)

#define IS_VAR(node)                \
    (node &&                        \
     (node)->data.type == VAR_TYPE)

#define OF_OP(node, opType)           \
    (IS_OP(node) &&                   \
     (node)->data.value.op == opType)

#define OF_NUM(node, i)                      \
    (IS_NUM(node) &&                         \
     doubleEqual((node)->data.value.num, i))

#define OF_VAR(node, i)           \
    (IS_VAR(node) &&              \
     (node)->data.value.var == i)

//TODO more easier non-obvious check (for example priority is 3 but not power)
#define IS_SUPPORTED_FUNC(opType) \
        (opType == OP_COS  ||     \
         opType == OP_SIN  ||     \
         opType == OP_TAN  ||     \
         opType == OP_COT  ||     \
         opType == OP_LOG  ||     \
         opType == OP_LN   ||     \
         opType == OP_ASIN ||     \
         opType == OP_ACOS ||     \
         opType == OP_ATAN ||     \
         opType == OP_ACOT ||     \
         opType == OP_SINH ||     \
         opType == OP_COSH ||     \
         opType == OP_TANH ||     \
         opType == OP_COTH)

const char* getNodeTypeString(NodeType type);
const char* getOpTypeString(OpType type);
int getOpType(const char* string);
uint getOpTypeArgumentCount(OpType type);
uint getOpTypePriority(OpType type);
//Applies appropriate operation regarding a and b and returns the result.
//If the operation doesn't require a second parameter (e.g. cos(x)) then leave b as NAN
//or use default value for b as NAN
double applyOperation(OpType type, double a, double b = NAN);

#endif
