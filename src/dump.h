#ifndef DUMP_H
#define DUMP_H

#include <stdio.h>
#include "tree.h"

void treeDump(FILE* html, TreeRoot* root, const char* commentary, const char* filename, int line);
void nodeDump(FILE* html, TreeNode* node, const char* commentary, const char* filename, int line);

#define treeDump(file, root, commentary) \
        treeDump(file, root, commentary, __FILE__, __LINE__)
#define nodeDump(file, node, commentary) \
        nodeDump(file, node, commentary, __FILE__, __LINE__)

FILE* initHtmlLogFile();

#endif
