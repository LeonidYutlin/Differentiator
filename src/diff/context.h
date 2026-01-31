#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdio.h>
#include <math.h>
#include <sys/types.h>

const size_t MAX_VALUE_STRING_LENGTH = 16;

struct Variable {
  ulong hash = 0;
  char* str = NULL;
  double value = NAN;
};

struct Variables {
  Variable* items = NULL;
  size_t capacity = 0;
  size_t count = 0;
};

#include "../data_structures/tree/node.h"

Variables* varsAlloc(size_t initialCapacity, Error* status = NULL);
Error varsDestroy(struct Variables* vars); 
size_t regVar(Variables* vars, const char* varStr, Error* status = NULL); 
Variable* getVar(Variables* vars, size_t index, Error* status = NULL);
Variable* findVar(Variables* vars, const char* varStr, Error* status = NULL);
//NOTE: any error (e.g. NULL vars) and this will return false
bool ofVar(Variables* vars, TreeNode* node, const char* varStr);
Error setVarValue(Variables* vars, const char* varStr, double value);
Error varsVerify(Variables* vars);

struct Context {
  FILE* sink = NULL;
  Variables* vars = NULL;
  uint stepCount = 0;
};

Error contextInit(Context* context, size_t initialCapacity);
Error contextDestroy(Context* context);

Error contextVerify(Context* context);

#define OF_VAR(vars, node, i) ofVar(vars, node, i)

#endif
