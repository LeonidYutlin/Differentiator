#include "nodetype.h"
#include <string.h>

const char* getNodeTypeString(NodeType type) {
    switch (type) {
        case OP_TYPE:  return "OP";
        case NUM_TYPE: return "NUM";
        case VAR_TYPE: return "VAR";
        default:       return "UNKNOWN TYPE ERROR";
    }
    return "UNKNOWN TYPE ERROR";
}

const char* getOpTypeString(OpType type) {
    switch (type) {
        case OP_ADD:     return "+";
        case OP_SUB:    return "-";
        case OP_MUL: return "*";
        case OP_DIV:   return "/";
        case OP_POW:    return "^";
        default:       return "UNKNOWN TYPE ERROR";
    }
    return "UNKNOWN TYPE ERROR";
}

int getOpType(const char* str) {
    if (strcmp(str, "+") == 0)
        return OP_ADD;
    if (strcmp(str, "-") == 0)
        return OP_SUB;
    if (strcmp(str, "*") == 0)
        return OP_MUL;
    if (strcmp(str, "/") == 0)
        return OP_DIV;
    if (strcmp(str, "^") == 0)
        return OP_POW;
    return -1;
}

uint getOpTypeArgumentCount(OpType type) {
    switch (type) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_POW:   return 2;
        default:         return 2;
    }
    return 0;
}

uint getOpTypePriority(OpType type) {
    switch (type) {
        case OP_ADD:
        case OP_SUB:    return 1;
        case OP_MUL:
        case OP_DIV:   return 2;
        case OP_POW:    return 3;
        default:          return 0;
    }
    return 0;
}
