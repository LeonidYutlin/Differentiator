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
        case OP_ADD:  return "+";
        case OP_SUB:  return "-";
        case OP_MUL:  return "*";
        case OP_DIV:  return "/";
        case OP_POW:  return "^";
        case OP_SIN:  return "sin";
        case OP_COS:  return "cos";
        case OP_TAN:  return "tan";
        case OP_COT:  return "cot";
        case OP_LOG:  return "log";
        case OP_LN :  return "ln";
        case OP_ASIN: return "arcsin";
        case OP_ACOS: return "arccos";
        case OP_ATAN: return "arctan";
        case OP_ACOT: return "arccot";
        case OP_SINH: return "sinh";
        case OP_COSH: return "cosh";
        case OP_TANH: return "tanh";
        case OP_COTH: return "coth";
        default:     return "UNKNOWN TYPE ERROR";
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
    if (strcmp(str, "sin") == 0)
        return OP_SIN;
    if (strcmp(str, "cos") == 0)
        return OP_COS;
    if (strcmp(str, "tan") == 0 ||
        strcmp(str, "tg") == 0)
        return OP_TAN;
    if (strcmp(str, "cot") == 0 ||
        strcmp(str, "ctg") == 0)
        return OP_COT;
    if (strcmp(str, "log") == 0)
        return OP_LOG;
    if (strcmp(str, "ln") == 0)
        return OP_LN;
    if (strcmp(str, "arcsin") == 0)
        return OP_ASIN;
    if (strcmp(str, "arccos") == 0)
        return OP_ACOS;
    if (strcmp(str, "arctan") == 0 ||
        strcmp(str, "arctg") == 0)
        return OP_ATAN;
    if (strcmp(str, "arccot") == 0 ||
        strcmp(str, "arcctg") == 0)
        return OP_ACOT;
    if (strcmp(str, "sinh") == 0 ||
        strcmp(str, "sh") == 0)
        return OP_SINH;
    if (strcmp(str, "cosh") == 0 ||
        strcmp(str, "ch") == 0)
        return OP_COSH;
    if (strcmp(str, "tanh") == 0 ||
        strcmp(str, "th") == 0)
        return OP_TANH;
    if (strcmp(str, "coth") == 0 ||
        strcmp(str, "cth") == 0)
        return OP_COTH;
    return -1;
}

uint getOpTypeArgumentCount(OpType type) {
    switch (type) {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_POW:
        case OP_LOG: return 2;
        case OP_SIN:
        case OP_COS:
        case OP_TAN:
        case OP_COT:
        case OP_ASIN:
        case OP_ACOS:
        case OP_ATAN:
        case OP_ACOT:
        case OP_SINH:
        case OP_COSH:
        case OP_TANH:
        case OP_COTH:
        case OP_LN : return 1;
        default:     return 2;
    }
    return 0;
}

uint getOpTypePriority(OpType type) {
    switch (type) {
        case OP_ADD:
        case OP_SUB: return 1;
        case OP_MUL:
        case OP_DIV: return 2;
        case OP_POW:
        case OP_SIN:
        case OP_COS:
        case OP_TAN:
        case OP_COT:
        case OP_ASIN:
        case OP_ACOS:
        case OP_ATAN:
        case OP_ACOT:
        case OP_SINH:
        case OP_COSH:
        case OP_TANH:
        case OP_COTH:
        case OP_LOG:
        case OP_LN : return 3;
        default:     return 0;
    }
    return 0;
}
