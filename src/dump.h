#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>
#include "tree.h"

void treeDump(FILE* html, TreeRoot* root, const char* commentary, const char* filename, int line);
void nodeDump(FILE* html, TreeNode* node, const char* commentary, const char* filename, int line);
void nodeTexDump(FILE* tex, TreeNode* node);
void treeTexDump(FILE* tex, TreeRoot* root);

#define treeDump(file, root, commentary) \
        treeDump(file, root, commentary, __FILE__, __LINE__)
#define nodeDump(file, node, commentary) \
        nodeDump(file, node, commentary, __FILE__, __LINE__)

FILE* initTexLogFile();
void closeTexLogFile(FILE* file);
FILE* initHtmlLogFile();

#endif
