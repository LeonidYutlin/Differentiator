#include "node.h"
#include "../../misc/util.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static const char* NULL_STRING_REPRESENTATION   = "nil";
static size_t NULL_STRING_REPRESENTATION_LENGTH = strlen(NULL_STRING_REPRESENTATION);

static TreeNode* nodeReadRecursion(char* buf, size_t bufSize, size_t* pos,
                                   Error* status, size_t* nodeCount);

#define RETURN_WITH_STATUS(value, returnValue) \
        { \
        if (status) \
            *status = value; \
        return returnValue; \
        }

Error nodeInit(TreeNode* node, NodeUnit data, TreeNode* parent,
                    TreeNode* left, TreeNode* right) {
    if (!node)
        return InvalidParameters;
    if (node->left  ||
        node->right ||
        node->left)
        return AttemptedReinitialization;

    node->data   = data;
    node->parent = parent;
    node->left   = left;
    node->right  = right;

    return OK;
}

TreeNode*  nodeDynamicInit(NodeUnit data, TreeNode* parent,
                           TreeNode* left, TreeNode* right,
                           Error* status) {
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));
    if (!node) {
        free(node);
        RETURN_WITH_STATUS(FailMemoryAllocation, NULL);
    }
    *node = {};

    Error returnedStatus = nodeInit(node, data, parent, left, right);
    if (returnedStatus) {
        free(node);
        RETURN_WITH_STATUS(returnedStatus, NULL);
    }

    return node;
}

TreeNode* nodeRead(FILE* f, Error* status, size_t* nodeCount) {
    char* buffer = NULL;
    size_t bufferSize = 0;
    if (readBufferFromFile(f, &buffer, &bufferSize))
        RETURN_WITH_STATUS(FailMemoryAllocation, NULL);

    Error returnedStatus = OK;
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
                                   Error* status, size_t* nodeCount) {
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
        NodeUnit data = {};
        int charReadN = 0;
        if (isdigit(buf[*pos]) ||
            (buf[*pos] == '-' && isdigit(buf[*pos + 1]))) {
            if (sscanf(buf + *pos,
                       "%lg%n",
                       &data.value, &charReadN) != 1)
                DUMP_ERROR_RETURN("No valid value in node");
            data.type = NUM_TYPE;
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
                data.value = opType;
                data.type = OP_TYPE;
                *pos += (size_t)charReadN;
            } else if (charReadN == 1){
                data.value = buf[*pos];
                data.type = VAR_TYPE;
                *pos += (size_t)charReadN;
            } else {
                DUMP_ERROR_RETURN("Bad variable name in node (longer than 1 char?)");
            }
        }
        if (isnan(data.value))
            DUMP_ERROR_RETURN("No value in node");

        uint expectedChildN = data.type == OP_TYPE
                              ? getOpTypeArgumentCount((OpType)data.value)
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

        TreeNode* node = nodeDynamicInit(data, NULL, left, right, status);
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

Error nodeTraverseInfix(TreeNode* node,
                        int cb(TreeNode* node, void* data, uint level),
                        void* data, uint level) {
	if (!node)
        return OK;

	return nodeTraverseInfix(node->left, cb, data, level + 1) ||
           cb(node, data, level) ||
           nodeTraverseInfix(node->right, cb, data, level + 1);
}

Error nodeTraversePrefix(TreeNode* node,
                         int cb(TreeNode* node, void* data, uint level),
                         void* data, uint level) {
	if (!node)
        return OK;

	return cb(node, data, level) ||
           nodeTraversePrefix(node->left, cb, data, level + 1) ||
           nodeTraversePrefix(node->right, cb, data, level + 1);
}

Error nodePrintPrefix(FILE* f, TreeNode* node) {
    if (!node)
        return InvalidParameters;

    fputc('(', f);
    fprintf(f, "%lf", node->data.value);
    nodePrintPrefix(f, node->left);
    nodePrintPrefix(f, node->right);
    fputc(')', f);
    return OK;
}

Error nodePrintInfix(FILE* f, TreeNode* node) {
    if (!node)
        return InvalidParameters;

    fputc('(', f);
    nodePrintPrefix(f, node->left);
    fprintf(f, "%lf", node->data.value);
    nodePrintPrefix(f, node->right);
    fputc(')', f);
    return OK;
}

Error nodePrintPostfix(FILE* f, TreeNode* node) { ///// ?
    if (!node)
        return InvalidParameters;

    fputc('(', f);
    nodePrintPrefix(f, node->left);
    nodePrintPrefix(f, node->right);
    fprintf(f, "%lf", node->data.value);
    fputc(')', f);
    return OK;
}

TreeNode* nodeCopy(TreeNode* src, TreeNode* newParent, Error* status) {
    if (!src)
        RETURN_WITH_STATUS(InvalidParameters, NULL);

    Error returnedStatus = OK;
    TreeNode* copy = nodeDynamicInit({src->data.type, src->data.value}, newParent);
    if (src->left)
        copy->left = nodeCopy(src->left, copy);
    if (src->right)
        copy->right = nodeCopy(src->right, copy);

    if (returnedStatus) {
        nodeDestroy(copy, true);
        RETURN_WITH_STATUS(returnedStatus, NULL);
    }

    return copy;
}

void nodeFixParents(TreeNode* node) {
    if (!node)
        return;

    if (node->left) {
        node->left->parent  = node;
        nodeFixParents(node->left);
    }
    if (node->right) {
        node->right->parent = node;
        nodeFixParents(node->right);
    }
}

Error nodeDelete(TreeNode* node, bool isAlloced, size_t* nodeCount) {
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

Error nodeDestroy(TreeNode* node, bool isAlloced, size_t* nodeCount) {
    if (!node)
        return InvalidParameters;

    nodeDestroy(node->left, isAlloced);
    nodeDestroy(node->right, isAlloced);
    node->left   = NULL;
    node->right  = NULL;
    node->parent = NULL;
    node->data.value   = {};

    if (isAlloced)
        free(node);
    if (nodeCount)
        (*nodeCount)--;

    return OK;
}

#undef RETURN_WITH_STATUS
