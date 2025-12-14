#include "../data_structures/tree/tree.h"
#include "io.h"
#include "../misc/util.h"
#include "../misc/quotes.h"
#include <time.h>
#include <assert.h>
#include <string.h>

static void nodeToTexTraverse(TreeNode* node, FILE* f, size_t* writtenCount);
static bool compareParentPriority(TreeNode* node);
static uint stepCount = 0;
static const size_t MAX_CHAR_PER_LINE = 64;

void nodeToTex(FILE* f, TreeNode* node) {
    assert(f);

    ++stepCount;

    fprintf(f,
            "\\raggedright(%u):\\begin{align*}\n",
            stepCount);
    size_t writtenCount = 0;
    nodeToTexTraverse(node, f, &writtenCount);
    fputs("\n\\end{align*}\\\\\n", f);
}

TreeNode* differentiationStepToTex(FILE* f, char var, TreeNode* before, TreeNode* after) {
    assert(f);
    nodeFixParents(after);

    ++stepCount;
    size_t nodeCount = 0;
    nodeTraverseInfix(before, countNodesCallback, &nodeCount);
    nodeTraverseInfix(after, countNodesCallback, &nodeCount);
    // double shrinkCoefficient = ((double)nodeCount - 40) * 0.002;
    // double size = 2.0 * (1 - (shrinkCoefficient < 0 ? 0 : shrinkCoefficient));

    // fprintf(f,
    //         "\\raggedright\\scalebox{1.5}{(%u):}\\begin{align*}\\scalebox{%lg}{$ \\frac{d}{d%c}",
    //         stepCount, size, var);
    fprintf(f,
            "\\raggedright(%u):\\begin{align*}\n\\frac{d}{d%c}",
            stepCount, var);
    size_t writtenCount = 0;
    nodeToTexTraverse(before, f, &writtenCount);
    fputs(" = ", f);
    nodeToTexTraverse(after, f, &writtenCount);
    fputs("\n\\end{align*}\\\\\n", f);
    return after;
}

void treeToTex(FILE* f, TreeRoot* root) {
    assert(f);
    if (!root)
        return;

    nodeToTex(f, root->rootNode);
}

FILE* initTexLogFile() {
    time_t timeAbs = time(NULL);
    char* name = getTimestampedString(".log/", ".tex");
    if (!name)
        return NULL;

    FILE* f = fopen(name, "w");
    if (!f) {
        free(name);
        return NULL;
    }
    srand((uint)timeAbs);

    fprintf(f,
            "\\documentclass{article}"
            "\\usepackage{amsmath}"
            "\\usepackage{geometry}"
            "\\usepackage{graphicx}"
            //"\\usepackage{breqn}"
            // "\\usepackage[T2A]{fontenc}"
            // "\\usepackage[utf8]{inputenc}"
            // "\\usepackage[english,russian]{babel}"
            "\\usepackage[colorlinks=true, linkcolor=blue, urlcolor=blue]{hyperref}"
            "\\usepackage{enumitem}"
            "\\geometry{a4paper, margin=1in}"
            "\\begin{document}"
            "\\section{Differentiating stuff}"
            "%s\\\\"
            "{Quote: %s\\\\}",
            name + strlen(".log/"),
            QUOTES[(unsigned long)random()
                        % (sizeof(QUOTES) / sizeof(char *))]);
    free(name);
    return f;
}

void closeTexLogFile(FILE* file) {
    if (!file)
        return;

    fputs("\\end{document}",
          file);
    fclose(file);
}

#define ADD_TO_COUNT(x) \
        { \
        if (writtenCount) \
            *writtenCount += x; \
        }

static void nodeToTexTraverse(TreeNode* node, FILE* f, size_t* writtenCount) {
	if (!node || !f)
        return;

    // i dont use macros's IS_OP and IS_NUM because they will contain unneccessary null checks
    bool needsBrackets = (node->parent &&
                          ((node->data.type == NUM_TYPE && node->data.value < 0) ||
                          (node->data.type == OP_TYPE && compareParentPriority(node))));
    bool isDivision = OP_OF(node, OP_DIV);
    // is this an expression of type (smth)^(1/number)
    bool isRoot     = (!isDivision &&
                       OP_OF(node, OP_POW) &&
                       OP_OF(node->right, OP_DIV) &&
                       NUM_OF(node->right->left, 1) &&
                       IS_NUM(node->right->left));

    if (writtenCount && *writtenCount > MAX_CHAR_PER_LINE) {
        fputs("\\\\\n", f);
        *writtenCount = 0;
    }

    if (needsBrackets) {
        fputc('(', f);
        ADD_TO_COUNT(1);
    }

    if (isDivision) {
        fputs("\\frac{", f);
        nodeToTexTraverse(node->left, f, NULL);
        fputs("}{", f);
        nodeToTexTraverse(node->right, f, NULL);
        fputc('}', f);
    } else if (isRoot) {
        fputs("\\sqrt[", f);
        nodeToTexTraverse(node->right->right, f, writtenCount);
        fputs("]{", f);
        nodeToTexTraverse(node->left, f, writtenCount);
        fputc('}', f);
    } else {
	    nodeToTexTraverse(node->left, f, writtenCount);
        switch (node->data.type) {
            case NUM_TYPE:
                {
                    size_t written = 0;
                    fprintf(f, "%lg%ln", node->data.value, (long*)&written);
                    ADD_TO_COUNT(written);
                };
                break;
            case VAR_TYPE:
                fprintf(f, "%c", (int)node->data.value);
                ADD_TO_COUNT(1);
                break;
            case OP_TYPE:
                {
                    OpType opType = (OpType)node->data.value;
                    if (!(opType == OP_MUL &&
                          node->right &&
                          node->right->data.type == VAR_TYPE)) {
                        const char* opStr = getOpTypeString(opType);
                        bool isSupportedFunc = (opType == OP_COS ||
                                                opType == OP_SIN ||
                                                opType == OP_TAN ||
                                                opType == OP_COT);
                        fprintf(f, "%s%s", isSupportedFunc ? "\\" : "", opStr);
                        ADD_TO_COUNT(strlen(opStr));
                    }
                };
                break;
            default:
                fputs("Error: unknown node type", f);
        }
        nodeToTexTraverse(node->right, f, writtenCount);
    }

    if (needsBrackets) {
        fputc(')', f);
        ADD_TO_COUNT(1);
    }
}

#undef ADD_TO_COUNT

static bool compareParentPriority(TreeNode* node) {
    assert(node);
    OpType parentType = (OpType)node->parent->data.value;
    OpType ownType    = (OpType)node->data.value;
    // fprintf(stderr,
    //         "called to compare parent %s and child %s, the result is %d\n",
    //         getOpTypeString(parentType), getOpTypeString(ownType),
    //         ownType == OP_DIV || parentType == OP_DIV
    //         ? false
    //         : getOpTypePriority(parentType) > getOpTypePriority(ownType));


    return ownType == OP_DIV || parentType == OP_DIV
           ? false
           : getOpTypePriority(parentType) > getOpTypePriority(ownType);
}

