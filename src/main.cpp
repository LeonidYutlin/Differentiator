#include "ds/tree/tree.h"
#include "ds/tree/dump/dump.h"
#include "diff/derivative.h"
#include "diff/io/io.h"
#include "diff/io/parse.h"
#include "diff/context.h"

//TODO: adapt eval for trees (arcsin, sin, log...)!
//TODO: calculation with given var values
//TODO: Taylor Series
//TODO: partial derivative
//TODO: total derivative
int main() {
  FILE* f = fopen(".test/parse_test.txt", "r");
  if (!f)
    return 1;

  char* buffer = NULL;
  size_t bufferSize = 0;
  if (readBufferFromFile(f, &buffer, &bufferSize))
    return 1; 

  fclose(f);

  Context ctx = {0};
  contextInit(&ctx, 32);
  openTexFile(&ctx);

  TreeNode* tree = parseFormula(buffer, ctx.vars); 
  free(buffer);

  FILE* log = openHtmlLogFile();
  if (!log)
    return 1;

  nodeDump(log, ctx.vars, tree, "<b3>Read tree</b3>");
  nodeToTex(&ctx, tree);
  TreeNode* diffTreeX = differentiate(&ctx, tree, "x");
  nodeDump(log, ctx.vars, diffTreeX, "<b3> tree after diff </b3>");
  nodeToTex(&ctx, diffTreeX);

  fclose(log);
  nodeDestroy(tree, true);
  nodeDestroy(diffTreeX, true);
  return 0;

  // //Examples of using callbacks! Yay
  // NodePrintCallbackData prefixData = {
  //   .sink = stderr, 
  //   .c = '<'
  // };
  // NodePrintCallbackData infixData = {
  //   .sink = stderr, 
  //   .vars = ctx.vars
  // };
  // NodePrintCallbackData postfixData = {
  //   .sink = stderr, 
  //   .c = '>'
  // };
  // nodeTraverse(diffTreeY, 
  //              .prefix  = nodePutcCallback,
  //              .infix   = nodePrintCallback,
  //              .postfix = nodePutcCallback,
  //              .prefixData  = &prefixData,
  //              .infixData   = &infixData,
  //              .postfixData = &postfixData);
  // fprintf(stderr, "\n\n\n");
  // prefixData.vars = ctx.vars;
  // nodeTraverse(diffTreeY, 
  //              .prefix  = nodePutcAndPrintCallback,
  //              .postfix = nodePutcCallback,
  //              .prefixData  = &prefixData,
  //              .postfixData = &postfixData);
  //
  // treeDestroy(tree, true);
  // nodeDestroy(diffTreeX, true);
  // contextDestroy(&ctx);
  // closeHtmlLogFile(log);
  
  return 0;
}
