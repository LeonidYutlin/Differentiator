#include "nodedef.h"
#include "diff.h"
#include "output.h"
#include <assert.h>

#define RETURN_WITH_STATUS(value, returnValue) \
        { \
        if (status) \
            *status = value; \
        return returnValue; \
        }

#define D_CONST \
        nodeDynamicInit({NUM_TYPE, 0})
#define D_X \
        nodeDynamicInit({NUM_TYPE, 1})
#define D_L \
        differentiate(node->left, var, tex)
#define D_R \
        differentiate(node->right, var, tex)
#define C_L \
        nodeCopy(node->left, NULL, NULL)
#define C_R \
        nodeCopy(node->right, NULL, NULL)
#define ADD_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_ADD}, NULL, l, r)
#define SUB_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_SUB}, NULL, l, r)
#define MUL_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_MUL}, NULL, l, r)
#define DIV_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_DIV}, NULL, l, r)
#define SQ_(l) \
        nodeDynamicInit({OP_TYPE, OP_POW}, NULL, l, nodeDynamicInit({NUM_TYPE, 2}))

static TreeNode* differentiateRec(TreeNode* node, char var, FILE* tex);

TreeNode* differentiate(TreeNode* node, char var, FILE* tex) {
    if (!node)
        return NULL;

    TreeNode* diff = differentiateRec(node, var, tex);
    nodeFixParents(diff);
    return diff;
}

static TreeNode* differentiateRec(TreeNode* node, char var, FILE* tex) {
    if (!node)
        return NULL;

    nodeToTex(tex, node);

    if (node->data.type == NUM_TYPE ||
        (node->data.type == VAR_TYPE && (char)node->data.value != var)) {
        // fprintf(stderr, "Called const der, node->data.value char is %c and var is %c\n", (char)node->data.value, var);
        return D_CONST;
    }

    if (node->data.type == VAR_TYPE && (char)node->data.value == var) {
        //fprintf(stderr, "Called x der\n");
        return D_X;
    }

    if (node->data.type == OP_TYPE) {
        OpType opType = (OpType)node->data.value;
        switch (opType) {
            case OP_ADD: return ADD_(D_L, D_R);
            case OP_SUB: return SUB_(D_L, D_R);
            case OP_MUL: return ADD_(MUL_(D_L, C_R), MUL_(C_L, D_R));
            case OP_DIV: return DIV_(SUB_(MUL_(C_R, D_L), MUL_(D_R, C_L)), SQ_(C_R));
        }
    }

    return NULL;
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
