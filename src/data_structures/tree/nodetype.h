#ifndef NODE_TYPE_H
#define NODE_TYPE_H

#include <stddef.h>
#include <sys/types.h>
#include <math.h>
#include "../../misc/util.h"

const size_t MAX_VALUE_STRING_LENGTH = 16;

#define NODE_TYPE_LIST()          \
  X(UNKNOWN_TYPE, "UNKNOWN TYPE") \
  X(OP_TYPE,      "OP")           \
  X(NUM_TYPE,     "NUM")          \
  X(VAR_TYPE,     "VAR")

enum NodeType {
  #define X(enm, ...) enm,
  NODE_TYPE_LIST()
  #undef X
};

struct NodeTypeInfo{
  NodeType type = UNKNOWN_TYPE;
  const char* str = NULL;
};

const NodeTypeInfo* parseNodeType(NodeType type);

//NOTE:
//X(enum, "str", "alrstr", argc, prior, isSupp)
#define OP_TYPE_LIST()                        \
  X(OP_ADD,  "+",      NULL,     2, 1, false) \
  X(OP_SUB,  "-",      NULL,     2, 1, false) \
  X(OP_MUL,  "*",      NULL,     2, 2, false) \
  X(OP_DIV,  "/",      NULL,     2, 2, false) \
  X(OP_POW,  "^",      NULL,     2, 3, false) \
  X(OP_SIN,  "sin",    NULL,     1, 3, true)  \
  X(OP_COS,  "cos",    NULL,     1, 3, true)  \
  X(OP_TAN,  "tan",    "tg",     1, 3, true)  \
  X(OP_COT,  "cot",    "ctg",    1, 3, true)  \
  X(OP_LOG,  "log",    NULL,     2, 3, true)  \
  X(OP_LN,   "ln",     NULL,     1, 3, true)  \
  X(OP_ASIN, "arcsin", NULL,     1, 3, true)  \
  X(OP_ACOS, "arccos", NULL,     1, 3, true)  \
  X(OP_ATAN, "arctan", "arctg",  1, 3, true)  \
  X(OP_ACOT, "arccot", "arcctg", 1, 3, true)  \
  X(OP_SINH, "sinh",   "sh",     1, 3, true)  \
  X(OP_COSH, "cosh",   "ch",     1, 3, true)  \
  X(OP_TANH, "tanh",   "th",     1, 3, true)  \
  X(OP_COTH, "coth",   "cth",    1, 3, true)

enum OpType{
  #define X(enm, ...) enm,
  OP_TYPE_LIST()
  #undef X
};

struct OpTypeInfo{
  OpType type;
  const char* str = NULL;
  const char* alt = NULL;
  uint argCount = 0;
  uint priority = 0;
  bool isSupported = false;
};

const OpTypeInfo* parseOpType(OpType type);
int getOpType(const char* string);
///Applies appropriate operation regarding a and b and returns the result.
///If the operation doesn't require a second parameter (e.g. cos(x)) then leave b as NAN
///or use default value for b as NAN
double applyOperation(OpType type, double a, double b = NAN);

#define IS_OP(node)  ((node) && (node)->data.type == OP_TYPE)
#define IS_NUM(node) ((node) && (node)->data.type == NUM_TYPE)
#define IS_VAR(node) ((node) && (node)->data.type == VAR_TYPE)
#define OF_OP(node, opType) (IS_OP((node)) && (node)->data.value.op == (opType))
#define OF_NUM(node, i)     (IS_NUM((node)) && doubleEqual((node)->data.value.num, (i)))
#define OF_VAR(node, i)     (IS_VAR((node)) && (node)->data.value.var == (i))

#endif
