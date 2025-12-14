#include "diff.h"
#include "../io/io.h"
#include <assert.h>
#include <math.h>

#define NUM_(i) \
        nodeDynamicInit((NodeUnit){NUM_TYPE, (double)i})
#define D_CONST \
        NUM_(0)
#define D_X \
        NUM_(1)
#define D_(d) \
        differentiateRec(d, var, tex)
#define D_L \
        D_(node->left)
#define D_R \
        D_(node->right)
#define C_(c) \
        nodeCopy(c, NULL, NULL)
#define C_L \
        C_(node->left)
#define C_R \
        C_(node->right)
#define ADD_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_ADD}, NULL, l, r)
#define SUB_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_SUB}, NULL, l, r)
#define MUL_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_MUL}, NULL, l, r)
#define DIV_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_DIV}, NULL, l, r)
#define POW_(l, r) \
        nodeDynamicInit({OP_TYPE, OP_POW}, NULL, l, r)
#define SQ_(l) \
        POW_(l, NUM_(2))
#define NEG_(r) \
        nodeDynamicInit({OP_TYPE, OP_MUL}, NULL, NUM_(-1), r)
#define INV_(r) \
        nodeDynamicInit({OP_TYPE, OP_DIV}, NULL, NUM_(1), r)
#define NEG_INV_(r) \
        nodeDynamicInit({OP_TYPE, OP_DIV}, NULL, NUM_(-1), r)
#define CHAIN_RULE_R(l) \
        MUL_(l, D_R)
#define CHAIN_RULE_L(l) \
        MUL_(l, D_L)
#define SIN_(r) \
        nodeDynamicInit({OP_TYPE, OP_SIN}, NULL, NULL, r)
#define COS_(r) \
        nodeDynamicInit({OP_TYPE, OP_COS}, NULL, NULL, r)
#define LN_(r) \
        nodeDynamicInit({OP_TYPE, OP_LN}, NULL, NULL, r)
#define SINH_(r) \
        nodeDynamicInit({OP_TYPE, OP_SINH}, NULL, NULL, r)
#define COSH_(r) \
        nodeDynamicInit({OP_TYPE, OP_COSH}, NULL, NULL, r)
#define SQRT_(l) \
        nodeDynamicInit({OP_TYPE, OP_POW}, NULL, l, INV_(NUM_(2)))

static TreeNode* differentiateRec(TreeNode* node, char var, FILE* tex);
static TreeNode* differentiatePower(TreeNode* node, char var, FILE* tex);

TreeNode* differentiate(TreeNode* node, char var, FILE* tex) {
    if (!node)
        return NULL;

    if (tex) {
        fputs("A derivative of this expression is deemed quite trivial:\\\\", tex);
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
            case OP_ADD:  DUMP_TO_TEX_AND_RETURN(ADD_(D_L, D_R));
            case OP_SUB:  DUMP_TO_TEX_AND_RETURN(SUB_(D_L, D_R));
            case OP_MUL:  DUMP_TO_TEX_AND_RETURN(ADD_(MUL_(D_L, C_R), MUL_(C_L, D_R)));
            case OP_DIV:  DUMP_TO_TEX_AND_RETURN(DIV_(SUB_(MUL_(C_R, D_L), MUL_(D_R, C_L)), SQ_(C_R)));
            case OP_POW:  DUMP_TO_TEX_AND_RETURN(differentiatePower(node, var, tex));
            case OP_SIN:  DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(COS_(C_R)));
            case OP_COS:  DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(NEG_(SIN_(C_R))));
            case OP_TAN:  DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(INV_(SQ_(COS_(C_R)))));
            case OP_COT:  DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(NEG_INV_(SQ_(SIN_(C_R)))));
            case OP_LOG:  DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(INV_(MUL_(C_R, LN_(C_L)))));
            case OP_LN :  DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(INV_(C_R)));
            case OP_SINH: DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(COSH_(C_R)));
            case OP_COSH: DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(SINH_(C_R)));
            case OP_TANH: DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(INV_(SQ_(COSH_(C_R)))));
            case OP_COTH: DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(NEG_INV_(SQ_(SINH_(C_R)))));
            case OP_ASIN: DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(INV_(SQRT_(SUB_(NUM_(1), SQ_(C_R))))));
            case OP_ACOS: DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(NEG_INV_(SQRT_(SUB_(NUM_(1), SQ_(C_R))))));
            case OP_ATAN: DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(INV_(ADD_(NUM_(1), SQ_(C_R)))));
            case OP_ACOT: DUMP_TO_TEX_AND_RETURN(CHAIN_RULE_R(NEG_INV_(ADD_(NUM_(1), SQ_(C_R)))));
        }
    }

    return NULL;
}

#undef DUMP_TO_TEX_AND_RETURN

TreeNode* differentiatePower(TreeNode* node, char var, FILE* tex) {
    if (!node ||
        !node->left ||
        !node->right)
        return NULL;

    bool leftContainsX = nodeTraverseInfix(node->left, findVariableCallback, (void*)&var);
    bool rightContainsX = nodeTraverseInfix(node->right, findVariableCallback, (void*)&var);

    if (!leftContainsX &&
        !rightContainsX)
        return D_CONST;

    if (leftContainsX &&
        !rightContainsX)
        return CHAIN_RULE_L(MUL_(C_R, POW_(C_L, (SUB_(C_R, NUM_(1))))));

    if (!leftContainsX &&
        rightContainsX) {
        if (NUM_OF(node->left, M_E))
            return CHAIN_RULE_R(C_(node));
        return CHAIN_RULE_R(MUL_(C_(node), LN_(C_L)));
    }

    if (leftContainsX &&
        rightContainsX) {
        TreeNode* temp = MUL_(C_R, LN_(C_L));
        nodeFixParents(temp);
        TreeNode* result = MUL_(C_(node), D_(temp));
        nodeDestroy(temp, true);
        return result;
    }
    return NULL;
}

#undef D_CONST
#undef D_X
#undef D_
#undef D_L
#undef D_R
#undef ADD_
#undef SUB_
#undef MUL_
#undef DIV_
#undef POW_
#undef SQ_
#undef INV_
#undef NEG_
#undef NEG_INV_
#undef SIN_
#undef COS_
#undef LN_
#undef COSH_
#undef SINH_
#undef SQRT_
#undef NUM_
#undef C_
#undef C_L
#undef C_R
