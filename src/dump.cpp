#include "dump.h"
#include "treedef.h"
#undef treeDump
#undef nodeDump

#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

static const size_t MAX_TIMESTAMPED_FILE_PATH_LENGTH = 128;
static const size_t MAX_IMAGE_FILE_PATH_LENGTH = 128;
static const size_t MAX_DOT_COMMAND_LENGTH = 512;

static const char* BG_COLOR      = "#FFFFFF";
static const char* BAD_OUTLINE   = "#602222";
static const char* BAD_FILL      = "#F02222";
static const char* DEFAULT_CELL  = "#F02222";
static const char* OP_CELL       = "#98D26B";
static const char* NUM_CELL      = "#7CA0CE";
static const char* VAR_CELL      = "#8673BA";
static const char* TABLE_OUTLINE = "#101510";
static const char* TABLE_FILL    = "#10151034";
static const char* ADDRESS_FILL  = "#10151034";
static const char* LEFT_FILL     = "#10151034";
static const char* RIGHT_FILL    = "#10151034";
static const char* PARENT_FILL   = "#10151034";
static const char* VALUE_FILL    = "#10151034";
static const char* TYPE_FILL     = "#10151034";
static const char* OK_EDGE       = "#2222E0";
static const char* BAD_EDGE      = "#E02222";
static const char* ROOT_OUTLINE  = "#666666";
static const char* ROOT_FILL     = "#DDDDDD";

static const char* QUOTES[] = {
    "Canary Canary Do Thy Hear Me ?",
    "SegFault? Not on my watch, bucko",
    "Aaaaand what has went wrong now?",
    "None of these words are in the Bible",
    "Et tu, Brute?",
    "Appledog",
    "Is my tree gonna make it? Prolly not",
    "OUR FOOD KEEPS BLOWING U-",
    "One must imagine treeDump happy...",
    "Boy oh boy I sure hope nothing bad has happened to my tree!",
    "I ATE IT ALL",
    "Ouch! That hurt!",
    "So what are you gonna say at my funeral now that you've killed me",
    "Crawling back to me, hm?",
    "Let me break it down for you",
    "So, this is actually quite simple!",
    "Me when I when when you when I when he at the",
    "You. Me. Gas station",
    "Everything's connected",
    "Redesign your graph. We know what we're doing.",
    "\"This task is quite simple\" My ass",
    "7 handshakes theory frfr",
    "I probably should not be writing this instead of the actual code",
    "She data on my structures till i linearize",
    "They say I've got a green thumb",
    "The fares are... bountiful?",
    "It isn't the first time an artist was eaten",
};

static char* getTimestampedString(const char* prefix, const char* suffix, uint count = 0);
static int treeTextDump(FILE* f, TreeRoot* root,
                        const char* commentary, const char* filename, int line,
                        uint callCount);
static int treeGraphDump(FILE* f, TreeRoot* root, uint callCount);
static void declareNode(FILE* dot, TreeNode* node, bool bondFailed = false);
static void executeDot(FILE* f, uint callCount, char* dotPath);
static void nodeTexDumpTraverse(TreeNode* node, FILE* f);
static bool compareParentPriority(TreeNode* node);

#define WARNING_PREFIX(condition) condition ? "<b><body><font color=\"red\">[!]</font></body></b>" : ""

void nodeTexDump(FILE* f, TreeNode* node) {
    assert(f);

    static uint callCount = 0;
    ++callCount;

    fprintf(f,
            "(%u): \\\\"
            "\\center $\\scalebox{2.5}{$",
            callCount);
    nodeTexDumpTraverse(node, f);
    fputs("$}$\\\\", f);
}

void treeTexDump(FILE* f, TreeRoot* root) {
    assert(f);
    if (!root)
        return;

    nodeTexDump(f, root->rootNode);
}

void treeDump(FILE* f, TreeRoot* root, const char* commentary, const char* filename, int line) {
    assert(f);
    assert(filename);
    assert(commentary);

    static uint callCount = 0;
    ++callCount;

    if (treeTextDump(f, root, commentary, filename, line, callCount))
        return;

    treeGraphDump(f, root, callCount);
}

#define DOT_HEADER_INIT(file) \
        fprintf(file, \
                 "digraph G {\n" \
                 "rankdir=TB;\n" \
                 "graph [bgcolor=\"%s\", pad=0.25, nodesep=0.55, " \
                        "ranksep=0.9, splines=ortho, ordering=\"in\"];\n" \
                 "node [shape=hexagon, style=\"filled\", color=\"%s\", penwidth=1.4, " \
                       "fillcolor=\"%s\", fontname=\"monospace\", fontsize=30];\n" \
                 "edge [color=\"%s\", penwidth=2.5, weight = 0, arrowsize=0.8, arrowhead=vee];\n", \
                 BG_COLOR, \
                 BAD_OUTLINE, BAD_FILL, \
                 BG_COLOR);

void nodeDump(FILE* f, TreeNode* node, const char* commentary, const char* filename, int line) {
    assert(f);
    assert(filename);
    assert(commentary);

    static uint callCount = 0;
    ++callCount;

    if (!node) {
        fprintf(f,
            "%s\n"
            "Textual Dump:\n"
            "TreeNode Dump #%u called from %s:%d\n"
            "TreeNode [NULL] {}\n",
            commentary,
            ++callCount, filename, line);
        return;
    }

    fprintf(f,
            "%s\n"
            "TreeNode Dump #%u called from %s:%d\n",
            commentary,
            callCount++, filename, line);

    char* dotPath = getTimestampedString(".log/dot-", ".txt", callCount);
    if (!dotPath) {
        fprintf(f, "<h1><b>Dot file name composition failed for this graph dump</h1><b>\n");
        return;
    }

    FILE* dot = fopen(dotPath, "w");
    if (!dot) {
        fprintf(f, "<h1><b>Dot file open failed for this graph dump</h1><b>\n");
        free(dotPath);
        return;
    }

    DOT_HEADER_INIT(dot);
    declareNode(dot, node);
    fprintf(dot, "}\n");
    fclose(dot);

    fputs("Graphical Dump:\n", f);
    executeDot(f, callCount, dotPath);
    free(dotPath);
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
            "\\begin{document}"
            "\\section{Differentiating stuff}"
            "%s\\\\"
            "Quote: %s\\\\",
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

FILE* initHtmlLogFile() {
    time_t timeAbs = time(NULL);
    char* name = getTimestampedString(".log/", ".html");
    if (!name)
        return NULL;

    FILE* f = fopen(name, "w");
    if (!f) {
        free(name);
        return NULL;
    }
    srand((uint)timeAbs);

    fprintf(f,
            "<pre><h1>%s</h1>\n"
            "<p><h3><i>[Q] %s\n</i></h3></p>",
            name + strlen(".log/"),
            QUOTES[(unsigned long)random()
                        % (sizeof(QUOTES) / sizeof(char *))]);
    free(name);
    return f;
}

static void nodeTexDumpTraverse(TreeNode* node, FILE* f) {
	if (!node || !f)
        return;

    bool needsBrackets = (node->parent &&
                         ((node->type == NUM_TYPE && node->data < 0) ||
                         (node->type == OP_TYPE && compareParentPriority(node))));
    bool isDivision = (node->type == OP_TYPE &&
                       (OpType)node->data == OP_DIVIDE);

    if (needsBrackets) fputc('(', f);
    if (isDivision)    fputs("\\frac{", f);
	nodeTexDumpTraverse(node->left, f);
    if (isDivision) {
        fputc('}', f);
    } else if (node->type == NUM_TYPE) {
        fprintf(f, "%lg", node->data);
    } else if (node->type == VAR_TYPE) {
        fprintf(f, "%c", (int)node->data);
    } else {
        OpType opType = (OpType)node->data;
        fputs(getOpTypeString(opType), f);
    }
    if (isDivision)    fputc('{', f);
    nodeTexDumpTraverse(node->right, f);
    if (isDivision)    fputc('}', f);
    if (needsBrackets) fputc(')', f);
}

static bool compareParentPriority(TreeNode* node) {
    assert(node);
    OpType parentType = (OpType)node->parent->data;
    OpType ownType    = (OpType)node->data;
    // fprintf(stderr,
    //         "called to compare parent %s and child %s, the result is %d\n",
    //         getOpTypeString(parentType), getOpTypeString(ownType),
    //         ownType == OP_DIVIDE || parentType == OP_DIVIDE
    //         ? false
    //         : getOpTypePriority(parentType) > getOpTypePriority(ownType));


    return ownType == OP_DIVIDE || parentType == OP_DIVIDE
           ? false
           : getOpTypePriority(parentType) > getOpTypePriority(ownType);
}

static int treeTextDump(FILE* f, TreeRoot* root,
                 const char* commentary, const char* filename, int line,
                 uint callCount) {
    if (!root) {
        fprintf(f,
            "%s\n"
            "Textual Dump:\n"
            "TreeRoot Dump #%u called from %s:%d\n"
            "TreeRoot [NULL] {}\n",
            commentary,
            callCount, filename, line);
        return -1;
    }

    fprintf(f,
            "%s\n"
            "TreeRoot Dump #%u called from %s:%d\n"
            "Textual Dump:\n"
            "TreeRoot [%p] {\n"
            "\tnodeCount = %lu\n"
            "\t%sstatus = %d\n"
            "\t%srootNode = %p\n"
            "}\n",
            commentary,
            callCount, filename, line,
            root,
            root->nodeCount,
            WARNING_PREFIX(root->status), root->status,
            WARNING_PREFIX(!root->rootNode), root->rootNode);

    return !root->rootNode ? -1 : 0;
}

static int treeGraphDump(FILE* f, TreeRoot* root, uint callCount) {
    assert(f);
    assert(root);

    char* dotPath = getTimestampedString(".log/dot-", ".txt", callCount);
    if (!dotPath) {
        fprintf(f, "<h1><b>Dot file name composition failed for this graph dump</h1><b>\n");
        return -1;
    }

    FILE* dot = fopen(dotPath, "w");
    if (!dot) {
        fprintf(f, "<h1><b>Dot file open failed for this graph dump</h1><b>\n");
        free(dotPath);
        return -1;
    }

    DOT_HEADER_INIT(dot);

    fprintf(dot,
            "root [shape=box, style=\"filled\", color=\"%s\", "
            "fillcolor=\"%s\", fontsize=14, label="
            "<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\" color=\"%s\">"
            "<tr>"
                "<td bgcolor=\"%s\"><b>ROOT</b></td>"
            "</tr>"
            "<tr>"
            "<td bgcolor=\"%s\"><b>nodeCount:</b> %lu</td>"
            "</tr>"
            "<tr>"
                "<td bgcolor=\"%s\"><b>status:</b> %d</td>"
            "</tr>"
            "<tr>"
                "<td bgcolor=\"%s\"><b>address:</b> %p</td>"
            "</tr>"
            "<tr>"
                "<td bgcolor=\"%s\"><b>rootNode:</b> %p</td>"
            "</tr>"
            "</table>"
            ">];\n",
            ROOT_OUTLINE, ROOT_FILL,
            TABLE_OUTLINE,
            TABLE_FILL,
            TABLE_FILL, root->nodeCount,
            TABLE_FILL, root->status,
            TABLE_FILL, root,
            TABLE_FILL, root->rootNode);

    declareNode(dot, root->rootNode);

    fprintf(dot, "root -> node%p [color=\"%s\"]\n", root->rootNode, OK_EDGE);
    fprintf(dot, "}\n");
    fclose(dot);

    fputs("Graphical Dump:\n", f);
    executeDot(f, callCount, dotPath);
    free(dotPath);

    return 0;
}

#undef DOT_HEADER_INIT

#define DECLARE_CHILD_NODE(child) \
        { \
        if (child) { \
            if (child->parent == node) { \
                fprintf(dot, "node%p -> node%p [color=\"%s\", arrowtail=vee, dir=both]\n", node, child, OK_EDGE); \
                declareNode(dot, child); \
            } else { \
                fprintf(dot, "node%p -> node%p [color=\"%s\"]\n", node, child, BAD_EDGE); \
                declareNode(dot, child, true); \
            } \
        } \
        }

static void declareNode(FILE* dot, TreeNode* node, bool bondFailed) {
    assert(dot);
    if (!node)
        return;

    char valStr[MAX_VALUE_STRING_LENGTH] = {0};
    if (node->type == OP_TYPE) {
        snprintf(valStr, MAX_VALUE_STRING_LENGTH, "(%s)", getOpTypeString((OpType)node->data));
    } else if (node->type == VAR_TYPE) {
        snprintf(valStr, MAX_VALUE_STRING_LENGTH, "(%c)", (char)node->data);
    }
    fprintf(dot,
                "node%p"
                "[shape=box, style=\"rounded, filled\", color=\"%s\", fillcolor=\"%s\", penwidth=2.1, fontsize=14, label="
                "<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\" color=\"%s\">"
                "<tr>"
                    "<td colspan=\"6\" bgcolor=\"%s\"><b>parent:</b> %p</td>"
                "</tr>"
                "<tr>"
                    "<td colspan=\"6\" bgcolor=\"%s\"><b>type:</b> %s</td>"
                "</tr>"
                "<tr>"
                    "<td colspan=\"6\" bgcolor=\"%s\"><b>value:</b> %lf %s</td>"
                "</tr>"
                "<tr>"
                    "<td colspan=\"6\" bgcolor=\"%s\"><b>address:</b> %p</td>"
                "</tr>"
                "<tr>"
                    "<td bgcolor=\"%s\"><b>left:</b> %p</td>"
                    "<td bgcolor=\"%s\"><b>right:</b> %p</td>"
                "</tr>"
                "</table>"
                ">];\n",
                node,
                TABLE_OUTLINE,
                node->type == OP_TYPE  ? OP_CELL  :
                node->type == NUM_TYPE ? NUM_CELL :
                node->type == VAR_TYPE ? VAR_CELL :
                DEFAULT_CELL,
                TABLE_OUTLINE,
                PARENT_FILL,  node->parent,
                TYPE_FILL,    getNodeTypeString(node->type),
                VALUE_FILL,   node->data,
                valStr,
                ADDRESS_FILL, node,
                node->type != OP_TYPE && node->left
                ? DEFAULT_CELL
                : LEFT_FILL, node->left,
                node->type != OP_TYPE && node->right
                ? DEFAULT_CELL
                : RIGHT_FILL, node->right);
    if (node->parent && bondFailed)
        fprintf(dot, "node%p -> node%p [color=\"%s\"]\n", node, node->parent, BAD_EDGE);
    DECLARE_CHILD_NODE(node->left);
    DECLARE_CHILD_NODE(node->right);
}

#undef DECLARE_CHILD_NODE

static void executeDot(FILE* f, uint callCount, char* dotPath) {
    char cmd[MAX_DOT_COMMAND_LENGTH] = {0};
    char* imgPath = getTimestampedString(".log/graph-", ".svg", callCount);
    if (!imgPath) {
        fprintf(f, "<h1><b>Image file path composition failed for this graph dump!</h1><b>\n");
        return;
    }
    snprintf(cmd, MAX_DOT_COMMAND_LENGTH, "dot -T svg \"%s\" -o \"%s\"", dotPath, imgPath);
    system(cmd);

    fprintf(f, "<img src=\"./%s\"></img>\n", imgPath + strlen(".log/"));
    free(imgPath);
}

static char* getTimestampedString(const char* prefix, const char* suffix, uint count) {
    time_t timeAbs = time(NULL);
    tm* localTime = localtime(&timeAbs);
    char* name    = (char*)calloc(MAX_TIMESTAMPED_FILE_PATH_LENGTH, sizeof(char));
    char* pattern = (char*)calloc(MAX_TIMESTAMPED_FILE_PATH_LENGTH, sizeof(char));
    if (!pattern || !name) {
        free(name); free(pattern);
        return NULL;
    }

    if (count) {
        snprintf(pattern, MAX_TIMESTAMPED_FILE_PATH_LENGTH,
                 "%s%%d-%%m-%%Y-%%H:%%M:%%S-%u%s",
                 prefix,
                 count,
                 suffix);
    } else {
        snprintf(pattern, MAX_TIMESTAMPED_FILE_PATH_LENGTH,
                 "%s%%d-%%m-%%Y-%%H:%%M:%%S%s",
                 prefix,
                 suffix);
    }
    if (!strftime(name, MAX_TIMESTAMPED_FILE_PATH_LENGTH, pattern, localTime)){
        free(name); free(pattern);
        return NULL;
    }
    free(pattern);
    return name;
}

#undef WARNING_PREFIX

