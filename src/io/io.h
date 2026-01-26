#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "../data_structures/tree/tree.h"

void nodeToTex(FILE* tex, TreeNode* node);
void treeToTex(FILE* tex, TreeRoot* root);
TreeNode* differentiationStepToTex(FILE* f, char var, TreeNode* before, TreeNode* after);

FILE* initTexLogFile();
void closeTexLogFile(FILE* file);

#endif
