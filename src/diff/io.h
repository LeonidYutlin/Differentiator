#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "diff/context.h"
#include "data_structures/tree/tree.h"

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

Error openTexFile(Context* context);
Error closeTexFile(Context* context);

//Extension of NodePutcCallbackData that includes vars field
//which is needed for nodePrint
//This struct is a perfect suit for all 3 callbacks below
struct NodePrintCallbackData {
  FILE* sink = NULL;
  char c = 0; 
  Variables* vars = NULL; 
};

struct NodePutcCallbackData {
  FILE* sink = NULL;
  char c = 0; 
};

Error nodePutcCallback(TreeNode* node, void* data, uint level);
Error nodePrintCallback(TreeNode* node, void* data, uint level);
Error nodePutcAndPrintCallback(TreeNode* node, void* data, uint level);

Error nodePrint(FILE* f, Variables* vars, TreeNode* node); 

#endif
