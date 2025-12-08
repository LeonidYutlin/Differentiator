#include "treedef.h"
#include "output.h"
#include "util.h"
#include "quotes.h"
#include <time.h>
#include <assert.h>
#include <string.h>

static void nodeToTexTraverse(TreeNode* node, FILE* f);
static bool compareParentPriority(TreeNode* node);
static uint stepCount = 0;

void nodeToTex(FILE* f, TreeNode* node) {
    assert(f);

    ++stepCount;

    fprintf(f,
            "\\raggedright\\scalebox{1.5}{(%u):}\\center \\scalebox{2.5}{$ ",
            stepCount);
    nodeToTexTraverse(node, f);
    fputs("$}\\\\", f);
}

TreeNode* differentiationStepToTex(FILE* f, char var, TreeNode* before, TreeNode* after) {
    assert(f);
    nodeFixParents(after);

    ++stepCount;
    size_t nodeCount = 0;
    nodeTraverseInfix(before, countNodesCallback, &nodeCount);
    nodeTraverseInfix(after, countNodesCallback, &nodeCount);
    double shrinkCoefficient = ((double)nodeCount - 40) * 0.002;
    double size = 2.0 * (1 - (shrinkCoefficient < 0 ? 0 : shrinkCoefficient));

    fprintf(f,
            "\\raggedright\\scalebox{1.5}{(%u):}\\center \\scalebox{%lg}{$ \\frac{d}{d%c}",
            stepCount, size, var);
    nodeToTexTraverse(before, f);
    fprintf(f, " = ");
    nodeToTexTraverse(after, f);
    fputs("$}\\\\", f);
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

static void nodeToTexTraverse(TreeNode* node, FILE* f) {
	if (!node || !f)
        return;

    bool needsBrackets = (node->parent &&
                         ((node->data.type == NUM_TYPE && node->data.value < 0) ||
                         (node->data.type == OP_TYPE && compareParentPriority(node))));
    bool isDivision = (node->data.type == OP_TYPE &&
                       (OpType)node->data.value == OP_DIV);

    if (needsBrackets) fputc('\\left(', f);
    if (isDivision)    fputs("\\frac{", f);
	nodeToTexTraverse(node->left, f);
    if (isDivision) {
        fputc('}', f);
    } else if (node->data.type == NUM_TYPE) {
        fprintf(f, "%lg", node->data.value);
    } else if (node->data.type == VAR_TYPE) {
        fprintf(f, "%c", (int)node->data.value);
    } else {
        OpType opType = (OpType)node->data.value;
        fputs(getOpTypeString(opType), f);
    }
    if (isDivision)    fputc('{', f);
    nodeToTexTraverse(node->right, f);
    if (isDivision)    fputc('}', f);
    if (needsBrackets) fputc('\\right)', f);
}

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

