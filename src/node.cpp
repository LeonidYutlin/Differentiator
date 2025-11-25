#include "nodedef.h"
#include "file.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static const char* NULL_STRING_REPRESENTATION   = "nil";
static size_t NULL_STRING_REPRESENTATION_LENGTH = strlen(NULL_STRING_REPRESENTATION);

static TreeNode* nodeReadRecursion(char* buf, size_t bufSize, size_t* pos,
                                   TreeStatus* status, size_t* nodeCount);

#define RETURN_WITH_STATUS(value, returnValue) \
        { \
        if (status) \
            *status = value; \
        return returnValue; \
        }

TreeStatus nodeInit(TreeNode* node, NodeType type, NodeUnit data, TreeNode* parent,
                    TreeNode* left, TreeNode* right) {
    if (!node)
        return InvalidParameters;
    if (/*!isnan(node->data)  ||*/
        node->left  ||
        node->right ||
        node->left)
        return AttemptedReinitialization;

    node->type   = type;
    node->data   = data;
    node->parent = parent;
    node->left   = left;
    node->right  = right;

    return OK;
}

TreeNode*  nodeDynamicInit(NodeType type, NodeUnit data, TreeNode* parent,
                           TreeNode* left, TreeNode* right,
                           TreeStatus* status) {
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!node) {
        free(node);
        RETURN_WITH_STATUS(FailMemoryAllocation, NULL);
    }
    *node = {.data = NAN};

    TreeStatus returnedStatus = nodeInit(node, type, data, parent, left, right);
    if (returnedStatus) {
        free(node);
        RETURN_WITH_STATUS(returnedStatus, NULL);
    }

    return node;
}

TreeNode* nodeRead(FILE* f, TreeStatus* status, size_t* nodeCount) {
    char* buffer = NULL;
    size_t bufferSize = 0;
    if (readBufferFromFile(f, &buffer, &bufferSize))
        RETURN_WITH_STATUS(FailMemoryAllocation, NULL);

    TreeStatus returnedStatus = OK;
    size_t pos = 0;
    TreeNode* node = nodeReadRecursion(buffer, bufferSize, &pos, &returnedStatus, nodeCount);
    free(buffer);
    if (returnedStatus)
        RETURN_WITH_STATUS(returnedStatus, NULL);
    return node;
}

#define DUMP_ERROR_RETURN(commentary) \
        { \
        fprintf(stderr, \
                "[ERROR]: Failed to read node at position %lu\n" \
                "Comment: %s\n" \
                "\tLine snippet:\n" \
                "\t->%.10s...\n", \
                *pos, \
                commentary, \
                buf + *pos); \
        RETURN_WITH_STATUS(FailReadNode, NULL); \
        }

#define SKIP_WHITESPACE \
        while (isspace(buf[*pos])) { \
            (*pos)++; \
        } \

static TreeNode* nodeReadRecursion(char* buf, size_t bufSize, size_t* pos,
                                   TreeStatus* status, size_t* nodeCount) {
    if (!buf || *pos >= bufSize)
        RETURN_WITH_STATUS(InvalidParameters, NULL);

    // fprintf(stderr,
    //             "[INFO]: at position %lu\n"
    //             "\tLine snippet:\n"
    //             "\t->%.10s...\n",
    //             *pos,
    //             buf + *pos);
    SKIP_WHITESPACE;
    if (buf[*pos] == '(') {
        (*pos)++;
        SKIP_WHITESPACE;
        double val = NAN;
        NodeType type = NUM_TYPE;
        int charReadN = 0;
        if (isdigit(buf[*pos]) ||
            (buf[*pos] == '-' && isdigit(buf[*pos + 1]))) {
            if (sscanf(buf + *pos,
                       "%lg%n",
                       &val, &charReadN) != 1)
                DUMP_ERROR_RETURN("No valid value in node");
            type = NUM_TYPE;
            *pos += (size_t)charReadN;
        } else {
            char valStr[MAX_VALUE_STRING_LENGTH] = {0};
            if (sscanf(buf + *pos,
                       "%s%n",
                       valStr, &charReadN) != 1)
                DUMP_ERROR_RETURN("No valid var/op value in node");
            int opType = getOpType(valStr);
            //fprintf(stderr, "returned opType %d\n", opType);
            if (opType >= 0) {
                val = opType;
                type = OP_TYPE;
                *pos += (size_t)charReadN;
            } else if (charReadN == 1){
                val = buf[*pos];
                type = VAR_TYPE;
                *pos += (size_t)charReadN;
            } else {
                DUMP_ERROR_RETURN("Bad variable name in node (longer than 1 char?)");
            }
        }
        if (isnan(val))
            DUMP_ERROR_RETURN("No value in node");

        uint expectedChildN = type == OP_TYPE
                              ? getOpTypeArgumentCount((OpType)val)
                              : 0;

        TreeNode* left  = nodeReadRecursion(buf, bufSize, pos, status, nodeCount);
        if (*status) {
            nodeDestroy(left, true);
            return NULL;
        }
        TreeNode* right = nodeReadRecursion(buf, bufSize, pos, status, nodeCount);
        if (*status) {
            //Я знаю что тут всегда ноды и так нулевой указатель
            //однако на будущие случаи лучше иметь деструктор чем не иметь
            nodeDestroy(left, true);
            nodeDestroy(right, true);
            return NULL;
        }

        uint childN = 0;
        if (left)  childN++;
        if (right) childN++;

        if (expectedChildN != childN) {
            nodeDestroy(left, true);
            nodeDestroy(right, true);
            DUMP_ERROR_RETURN("Node has too few or too many null children!");
        }

        SKIP_WHITESPACE;
        if (buf[*pos] == ')') {
            (*pos)++;
        } else {
            nodeDestroy(left, true);
            nodeDestroy(right, true);
            DUMP_ERROR_RETURN("No closing parenthesis");
        }

        TreeNode* node = nodeDynamicInit(type, val, NULL, left, right, status);
        if (*status) {
            nodeDestroy(left, true);
            nodeDestroy(right, true);
            nodeDestroy(node, true);
            return NULL;
        }

        if (node->left)
            node->left->parent  = node;
        if (node->right)
            node->right->parent = node;

        if (nodeCount)
            (*nodeCount)++;
        return node;
    } else if (strncmp(buf + *pos,
                       NULL_STRING_REPRESENTATION,
                       NULL_STRING_REPRESENTATION_LENGTH) == 0) {
        *pos += NULL_STRING_REPRESENTATION_LENGTH;
        return NULL;
    }

    DUMP_ERROR_RETURN("Illegal character at the start of a node declaration");
}

#undef DUMP_ERROR_RETURN
#undef SKIP_WHITESPACE

int nodeTraverse(TreeNode* node,
                 int cb(TreeNode* node, void* data, uint level),
                 void* data, uint level) {
	if (!node)
        return OK;

	return nodeTraverse(node->left, cb, data, level + 1) ||
           cb(node, data, level) ||
           nodeTraverse(node->right, cb, data, level + 1);
}

TreeStatus nodePrintPrefix(FILE* f, TreeNode* node) {
    if (!node)
        return InvalidParameters;

    fputc('(', f);
    fprintf(f, "%lf", node->data);
    nodePrintPrefix(f, node->left);
    nodePrintPrefix(f, node->right);
    fputc(')', f);
    return OK;
}

TreeStatus nodePrintInfix(FILE* f, TreeNode* node) {
    if (!node)
        return InvalidParameters;

    fputc('(', f);
    nodePrintPrefix(f, node->left);
    fprintf(f, "%lf", node->data);
    nodePrintPrefix(f, node->right);
    fputc(')', f);
    return OK;
}

TreeStatus nodePrintPostfix(FILE* f, TreeNode* node) { /////
    if (!node)
        return InvalidParameters;

    fputc('(', f);
    nodePrintPrefix(f, node->left);
    nodePrintPrefix(f, node->right);
    fprintf(f, "%lf", node->data);
    fputc(')', f);
    return OK;
}

TreeStatus nodeDelete(TreeNode* node, bool isAlloced, size_t* nodeCount) {
    if (!node)
        return InvalidParameters;

    if (node->parent) {
        if (node->parent->left == node)
            node->parent->left = NULL;
        else if (node->parent->right == node)
            node->parent->right = NULL;
    }

    return nodeDestroy(node, isAlloced, nodeCount);
}

TreeStatus nodeDestroy(TreeNode* node, bool isAlloced, size_t* nodeCount) {
    if (!node)
        return InvalidParameters;

    nodeDestroy(node->left, isAlloced);
    nodeDestroy(node->right, isAlloced);
    node->left   = NULL;
    node->right  = NULL;
    node->parent = NULL;
    node->data   = 0;

    if (isAlloced)
        free(node);
    if (nodeCount)
        (*nodeCount)--;

    return OK;
}

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
        case OP_PLUS:     return "+";
        case OP_MINUS:    return "-";
        case OP_MULTIPLY: return "*";
        case OP_DIVIDE:   return "/";
        default:       return "UNKNOWN TYPE ERROR";
    }
    return "UNKNOWN TYPE ERROR";
}

int getOpType(const char* str) {
    if (strcmp(str, "+") == 0)
        return OP_PLUS;
    if (strcmp(str, "-") == 0)
        return OP_MINUS;
    if (strcmp(str, "*") == 0)
        return OP_MULTIPLY;
    if (strcmp(str, "/") == 0)
        return OP_DIVIDE;
    return -1;
}

uint getOpTypeArgumentCount(OpType type) {
    switch (type) {
        case OP_PLUS:
        case OP_MINUS:
        case OP_MULTIPLY:
        case OP_DIVIDE:   return 2;
        default:       return 2;
    }
    return 0;
}

#undef RETURN_WITH_STATUS
