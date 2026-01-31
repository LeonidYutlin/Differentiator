#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "context.h"
#include "../data_structures/tree/tree.h"

Error nodeToTex(Context* context, TreeNode* node);
Error treeToTex(Context* context, TreeRoot* root);
TreeNode* differentiationStepToTex(Context* context, const char* var, 
                                   TreeNode* before, TreeNode* after,
                                   Error* status = NULL);

//Dumps errors in stderr...
TreeNode* parseFormula(const char* expression);

//not context because file isnt the tex output file but instead a "source" file
TreeNode* nodeRead(FILE* file, Variables* vars, Error* status = NULL, size_t* nodeCount = NULL);
TreeRoot* treeRead(FILE* file, Variables* vars, Error* status = NULL);

Error nodePrintPrefix (FILE* file, Variables* vars, TreeNode* node);
Error nodePrintInfix  (FILE* file, Variables* vars, TreeNode* node);
Error nodePrintPostfix(FILE* file, Variables* vars, TreeNode* node);

Error treePrintPrefix (FILE* file, Variables* vars, TreeRoot* root);
Error treePrintInfix  (FILE* file, Variables* vars, TreeRoot* root);
Error treePrintPostfix(FILE* file, Variables* vars, TreeRoot* root);

Error openTexFile(Context* context);
Error closeTexFile(Context* context);

#endif
