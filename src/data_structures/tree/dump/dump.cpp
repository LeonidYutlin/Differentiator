#include "dump.h"
#include "../tree.h"
#undef treeDump
#undef nodeDump

#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "../../../misc/quotes.h"
#include "../../../misc/util.h"
#include "../../queue/queue.h"

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

static int treeTextDump(FILE* f, TreeRoot* root,
                        const char* commentary, const char* filename, int line,
                        uint callCount);
static int treeGraphDump(FILE* f, TreeRoot* root, uint callCount);
static void populateDot(FILE* dot, TreeNode* node);
static void declareNode(FILE* dot, TreeNode* node, bool bondFailed = false);
static void declareRank(FILE* dot, TreeNode* node, Queue** queue);
static void executeDot(FILE* f, uint callCount, char* dotPath);

#define WARNING_PREFIX(condition) condition ? "<b><body><font color=\"red\">[!]</font></body></b>" : ""

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

#define DOT_HEADER_INIT(file)                                                                      \
        fprintf(file,                                                                              \
                 "digraph G {\n"                                                                   \
                 "rankdir=TB;\n"                                                                   \
                 "graph [bgcolor=\"%s\", pad=0.25, nodesep=0.55, "                                 \
                        "ranksep=0.9, splines=ortho, ordering=\"in\"];\n"                          \
                 "node [shape=hexagon, style=\"filled\", color=\"%s\", penwidth=1.4, "             \
                       "fillcolor=\"%s\", fontname=\"monospace\", fontsize=30];\n"                 \
                 "edge [color=\"%s\", penwidth=2.5, weight = 0, arrowsize=0.8, arrowhead=vee];\n", \
                 BG_COLOR,                                                                         \
                 BAD_OUTLINE, BAD_FILL,                                                            \
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
        fputs("<h1><b>Dot file name composition failed for this graph dump</h1><b>\n", f);
        return;
    }

    FILE* dot = fopen(dotPath, "w");
    if (!dot) {
        fputs("<h1><b>Dot file open failed for this graph dump</h1><b>\n", f);
        free(dotPath);
        return;
    }

    DOT_HEADER_INIT(dot);
    populateDot(dot, node);
    fputs("}\n", dot);
    fclose(dot);

    fputs("Graphical Dump:\n", f);
    executeDot(f, callCount, dotPath);
    free(dotPath);
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
        fputs("<h1><b>Dot file name composition failed for this graph dump</h1><b>\n", f);
        return -1;
    }

    FILE* dot = fopen(dotPath, "w");
    if (!dot) {
        fputs("<h1><b>Dot file open failed for this graph dump</h1><b>\n", f);
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

    populateDot(dot, root->rootNode);

    fprintf(dot, "root -> node%p [color=\"%s\"]\n", root->rootNode, OK_EDGE);
    fputs("}\n", dot);
    fclose(dot);

    fputs("Graphical Dump:\n", f);
    executeDot(f, callCount, dotPath);
    free(dotPath);

    return 0;
}

#undef DOT_HEADER_INIT

static void populateDot(FILE* dot, TreeNode* node) {
    assert(dot);
    assert(node);

    declareNode(dot, node);
    Queue* queue = NULL;
    declareRank(dot, node, &queue);
}

#define DECLARE_CHILD_NODE(child)                                                          \
        {                                                                                  \
        if (child) {                                                                       \
            if (child->parent == node) {                                                   \
                fprintf(dot, "node%p -> node%p [color=\"%s\", arrowtail=vee, dir=both]\n", \
                        node, child, OK_EDGE);                                             \
                declareNode(dot, child);                                                   \
            } else {                                                                       \
                fprintf(dot, "node%p -> node%p [color=\"%s\"]\n", node, child, BAD_EDGE);  \
                declareNode(dot, child, true);                                             \
            }                                                                              \
        }                                                                                  \
        }

static void declareNode(FILE* dot, TreeNode* node, bool bondFailed) {
    assert(dot);
    if (!node)
        return;

    char valStr[MAX_VALUE_STRING_LENGTH] = {0};
    if (node->data.type == OP_TYPE) {
        snprintf(valStr, MAX_VALUE_STRING_LENGTH, "(%s)", getOpTypeString((OpType)node->data.value));
    } else if (node->data.type == VAR_TYPE) {
        snprintf(valStr, MAX_VALUE_STRING_LENGTH, "(%c)", (char)node->data.value);
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
                node->data.type == OP_TYPE  ? OP_CELL  :
                node->data.type == NUM_TYPE ? NUM_CELL :
                node->data.type == VAR_TYPE ? VAR_CELL :
                DEFAULT_CELL,
                TABLE_OUTLINE,
                PARENT_FILL,  node->parent,
                TYPE_FILL,    getNodeTypeString(node->data.type),
                VALUE_FILL,   node->data.value,
                valStr,
                ADDRESS_FILL, node,
                node->data.type != OP_TYPE && node->left
                ? DEFAULT_CELL
                : LEFT_FILL, node->left,
                node->data.type != OP_TYPE && node->right
                ? DEFAULT_CELL
                : RIGHT_FILL, node->right);
    if (node->parent && bondFailed)
        fprintf(dot, "node%p -> node%p [color=\"%s\"]\n", node, node->parent, BAD_EDGE);
    DECLARE_CHILD_NODE(node->left);
    DECLARE_CHILD_NODE(node->right);
}

#undef DECLARE_CHILD_NODE

static void declareRank(FILE* dot, TreeNode* node, Queue** queue) {
    assert(dot);
    assert(node);
    assert(queue);

    fputs("{ rank = same; ", dot);
    Queue* newQueue = NULL;
    TreeNode* cur = node;
    do {
        fprintf(dot, "node%p; ", cur);
        if (cur->left)
            enqueue(&newQueue, cur->left);
        if (cur->right)
            enqueue(&newQueue, cur->right);
    } while (!dequeue(queue, &cur));
    fputs("}\n", dot);
    if (!dequeue(&newQueue, &cur))
        declareRank(dot, cur, &newQueue);
}

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

#undef WARNING_PREFIX

