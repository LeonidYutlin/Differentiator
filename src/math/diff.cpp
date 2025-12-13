#include "diff.h"
#include "../io/io.h"
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
        differentiateRec(node->left, var, tex)
#define D_R \
        differentiateRec(node->right, var, tex)
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

    if (tex) {
        fprintf(tex, "\\raggedright\\scalebox{1.5}{A derivative of this expression is deemed quite trivial:}\\\\");
        nodeToTex(tex, node);
    }

    TreeNode* diff = differentiateRec(node, var, tex);
    nodeFixParents(diff);
    return diff;
}

#define DUMP_TO_TEX_AND_RETURN(returnNode) \
        { \
        if (tex) { \
            return differentiationStepToTex(tex, var, node, returnNode); \
        } else { \
            return returnNode; \
        }\
        }\

static TreeNode* differentiateRec(TreeNode* node, char var, FILE* tex) {
    if (!node)
        return NULL;

    if (node->data.type == NUM_TYPE ||
        (node->data.type == VAR_TYPE && (char)node->data.value != var)) {
        // fprintf(stderr, "Called const der, node->data.value char is %c and var is %c\n", (char)node->data.value, var);
        DUMP_TO_TEX_AND_RETURN(D_CONST);
    }

    if (node->data.type == VAR_TYPE && (char)node->data.value == var) {
        //fprintf(stderr, "Called x der\n");
        DUMP_TO_TEX_AND_RETURN(D_X);
    }

    if (node->data.type == OP_TYPE) {
        OpType opType = (OpType)node->data.value;
        switch (opType) {
            case OP_ADD: DUMP_TO_TEX_AND_RETURN(ADD_(D_L, D_R));
            case OP_SUB: DUMP_TO_TEX_AND_RETURN(SUB_(D_L, D_R));
            case OP_MUL: DUMP_TO_TEX_AND_RETURN(ADD_(MUL_(D_L, C_R), MUL_(C_L, D_R)));
            case OP_DIV: DUMP_TO_TEX_AND_RETURN(DIV_(SUB_(MUL_(C_R, D_L), MUL_(D_R, C_L)), SQ_(C_R)));
            //case OP_POW: return diffPower()
        }
    }

    return NULL;
}

#undef DUMP_TO_TEX_AND_RETURN
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
