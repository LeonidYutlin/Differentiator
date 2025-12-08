#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "tree.h"

void nodeToTex(FILE* tex, TreeNode* node);
void treeToTex(FILE* tex, TreeRoot* root);
TreeNode* differentiationStepToTex(FILE* f, char var, TreeNode* before, TreeNode* after);

FILE* initTexLogFile();
void closeTexLogFile(FILE* file);

#endif
