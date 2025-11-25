#include "diff.h"
#include "nodedef.h"
#include "dump.h"
#include <assert.h>

#define RETURN_WITH_STATUS(value, returnValue) \
        { \
        if (status) \
            *status = value; \
        return returnValue; \
        }

#define D_CONST \
        nodeDynamicInit(NUM_TYPE, 0)
#define D_X \
        nodeDynamicInit(NUM_TYPE, 1)
#define D_L \
        differentiate(node->left, var, tex)
#define D_R \
        differentiate(node->right, var, tex)
#define C_L \
        nodeCopy(node->left,  NULL)
#define C_R \
        nodeCopy(node->right, NULL)
#define ADD_(l, r) \
        nodeDynamicInit(OP_TYPE, OP_PLUS, NULL, l, r)
#define SUB_(l, r) \
        nodeDynamicInit(OP_TYPE, OP_MINUS, NULL, l, r)
#define MUL_(l, r) \
        nodeDynamicInit(OP_TYPE, OP_MULTIPLY, NULL, l, r)
#define DIV_(l, r) \
        nodeDynamicInit(OP_TYPE, OP_DIVIDE, NULL, l, r)
#define SQ_(l) \
        nodeDynamicInit(OP_TYPE, OP_POWER, NULL, l, nodeDynamicInit(NUM_TYPE, 2))

static TreeNode* differentiateRec(TreeNode* node, char var, FILE* tex);
static void nodeRemedyParents(TreeNode* node);

TreeNode* differentiate(TreeNode* node, char var, FILE* tex) {
    if (!node)
        return NULL;

    TreeNode* diff = differentiateRec(node, var, tex);
    TreeNode* parent = NULL;
    nodeRemedyParents(diff);
    return diff;
}

static TreeNode* differentiateRec(TreeNode* node, char var, FILE* tex) {
    if (!node)
        return NULL;

    nodeTexDump(tex, node);

    if (node->type == NUM_TYPE ||
        (node->type == VAR_TYPE && (char)node->data != var)) {
        // fprintf(stderr, "Called const der, node->data char is %c and var is %c\n", (char)node->data, var);
        return D_CONST;
    }

    if (node->type == VAR_TYPE && (char)node->data == var) {
        //fprintf(stderr, "Called x der\n");
        return D_X;
    }

    if (node->type == OP_TYPE) {
        OpType opType = (OpType)node->data;
        switch (opType) {
            case OP_PLUS:     return ADD_(D_L, D_R);
            case OP_MINUS:    return SUB_(D_L, D_R);
            case OP_MULTIPLY: return ADD_(MUL_(D_L, C_R), MUL_(C_L, D_R));
            case OP_DIVIDE:   return DIV_(SUB_(MUL_(C_R, D_L), MUL_(D_R, C_L)), SQ_(C_R));
        }
    }

    return NULL;
}

static void nodeRemedyParents(TreeNode* node) {
    if (!node)
        return;

    if (node->left) {
        node->left->parent  = node;
        nodeRemedyParents(node->left);
    }
    if (node->right) {
        node->right->parent = node;
        nodeRemedyParents(node->right);
    }
}

#undef RETURN_WITH_STATUS
#undef D_CONST
#undef D_X
#undef D_L
#undef D_R
#undef ADD_
#undef SUB_
#undef MUL_
#undef DIV_
#undef SQ_
