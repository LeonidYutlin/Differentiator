#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "tree.h"

void nodeToTex(FILE* tex, TreeNode* node);
void treeToTex(FILE* tex, TreeRoot* root);

FILE* initTexLogFile();
void closeTexLogFile(FILE* file);

#endif
