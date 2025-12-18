#include "../data_structures/tree/tree.h"
#include "io.h"
#include "../misc/util.h"
#include "../misc/quotes.h"
#include <time.h>
#include <assert.h>
#include <string.h>

static void nodeToTexTraverse(TreeNode* node, FILE* f,
                              size_t* writtenCount,
                              bool suppressBrackets = false, bool suppressNewline = false);
static bool compareParentPriority(TreeNode* node);
static uint stepCount = 0;
static const size_t MAX_CHAR_PER_LINE = 54;

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
    fprintf(f,
            "\\raggedright(%u):\\begin{align*}\n\\frac{d}{d%c}",
            stepCount, var);
    size_t writtenCount = 0;
    nodeToTexTraverse(before, f, &writtenCount);
    fputs(" = ", f);
    nodeToTexTraverse(after, f, &writtenCount);
    fputs(" = ", f);
    nodeOptimize(&after);
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
            "\\usepackage[colorlinks=true, linkcolor=blue, urlcolor=blue]{hyperref}"
            "\\usepackage{enumitem}"
            "\\geometry{a4paper, margin=1in}"
            "\\DeclareMathOperator{\\arccot}{arccot}"
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
            *writtenCount += (size_t)x; \
        }

static void nodeToTexTraverse(TreeNode* node, FILE* f, size_t* writtenCount,
                              bool suppressBrackets, bool suppressNewline) {
	if (!node || !f)
        return;

    bool needsBrackets = (node->parent &&
                          !suppressBrackets &&
                          ((IS_NUM(node) && node->data.value < 0) ||
                            IS_SUPPORTED_FUNC((OpType)node->parent->data.value) ||
                           (IS_OP(node) && compareParentPriority(node))));
    bool isDivision = OF_OP(node, OP_DIV);
    bool isLog      = (!isDivision &&
                       OF_OP(node, OP_LOG));
    // is this an expression of type (smth)^(1/number)
    bool isRoot     = (!isDivision &&
                       !isLog &&
                       OF_OP(node, OP_POW) &&
                       OF_OP(node->right, OP_DIV) &&
                       OF_NUM(node->right->left, 1) &&
                       IS_NUM(node->right->left));

    if (needsBrackets) {
        fputc('(', f);
        ADD_TO_COUNT(1);
    }

    if (isDivision) {
        fputs("\\frac{", f);
        nodeToTexTraverse(node->left, f, writtenCount, true, true);
        fputs("}{", f);
        nodeToTexTraverse(node->right, f, writtenCount, true, true);
        fputc('}', f);
    } else if (isLog) {
        fputs("\\log_{", f);
        nodeToTexTraverse(node->left, f, writtenCount, true, true);
        fputs("}", f);
        nodeToTexTraverse(node->right, f, writtenCount, false, suppressNewline);
    } else if (isRoot) {
        if (doubleEqual(node->right->right->data.value, 2)) {
            fputs("\\sqrt{", f);
        } else {
            fputs("\\sqrt[", f);
            nodeToTexTraverse(node->right->right, f, writtenCount, true, true);
            fputs("]{", f);
        }
        nodeToTexTraverse(node->left, f, writtenCount, true, true);
        fputc('}', f);
    } else {
        bool isPow = OF_OP(node, OP_POW);
        if (isPow) fputc('{', f);
	    nodeToTexTraverse(node->left, f, writtenCount, suppressNewline);
        if (isPow) fputc('}', f);
        switch (node->data.type) {
            case NUM_TYPE: {
                long written = 0;
                fprintf(f, "%lg%ln", node->data.value, &written);
                ADD_TO_COUNT(written);
            }
            break;
            case VAR_TYPE: {
                fprintf(f, "%c", (int)node->data.value);
                ADD_TO_COUNT(1);
            }
            break;
            case OP_TYPE: {
                OpType opType = (OpType)node->data.value;
                if (!(opType == OP_MUL &&
                    node->right &&
                    node->right->data.type == VAR_TYPE)) {
                    long written = 0;
                    const char* opStr = getOpTypeString(opType);
                    fprintf(f, "%s%s%ln",
                                IS_SUPPORTED_FUNC(opType) ? "\\" : "", opStr, &written);
                    ADD_TO_COUNT(written);
                }
            }
            break;
            default:
                fputs("Error: unknown node type", f);
        }
        if (isPow) fputc('{', f);
        nodeToTexTraverse(node->right, f, writtenCount, isPow, isPow || suppressNewline);
        if (isPow) fputc('}', f);
    }

    if (needsBrackets) {
        fputc(')', f);
        ADD_TO_COUNT(1);
    }
    if (!suppressNewline &&
        writtenCount &&
        *writtenCount > MAX_CHAR_PER_LINE) {
        // fprintf(stderr, "Putting a newline! Currently i am inside %s (%lf) and my suppressNewline is %d\n",
        //         getOpTypeString((OpType)node->data.value), node->data.value, suppressNewline);
        fputs("\\\\\n", f);
        *writtenCount = 0;
    }
}

#undef ADD_TO_COUNT

static bool compareParentPriority(TreeNode* node) {
    assert(node);
    OpType parentType = (OpType)node->parent->data.value;
    OpType ownType    = (OpType)node->data.value;

    return getOpTypePriority(parentType) > getOpTypePriority(ownType);
}

