#include "data_structures/tree/tree.h"
#include "data_structures/tree/dump/dump.h"
#include "diff/derivative.h"
#include "diff/io.h"
#include "diff/context.h"

//TODO: adapt eval for trees !
//TODO: calculation with given var values
//TODO: Taylor Series
//TODO: partial derivative
//TODO: total derivative
int main() {
  //dumpErrors(stdout);

  FILE* log = openHtmlLogFile();

  Context ctx = {0};
  contextInit(&ctx, 32);
  openTexFile(&ctx);

  FILE* expr = fopen(".test/expr_test.txt", "r");
  TreeRoot* tree = treeRead(expr, ctx.vars);
  if (!tree) {
    fprintf(stderr, "IT IS NULL\n");
    return 1;
  }
  fclose(expr);
  treeDump(log, ctx.vars, tree, "<b3>Read tree</b3>");
  treeToTex(&ctx, tree);
  TreeNode* diffTreeX = differentiate(&ctx, tree->rootNode, "xanr");
  nodeDump(log, ctx.vars, diffTreeX, "<b3> tree after diff </b3>");
  nodeToTex(&ctx, diffTreeX);
  TreeNode* diffTreeY = differentiate(&ctx, tree->rootNode, "\\varepsilon");
  nodeDump(log, ctx.vars, diffTreeY, "<b3> tree after diff </b3>");
  nodeToTex(&ctx, diffTreeY);

  //Examples of using callbacks! Yay
  NodePrintCallbackData prefixData = {
    .sink = stderr, 
    .c = '<'
  };
  NodePrintCallbackData infixData = {
    .sink = stderr, 
    .vars = ctx.vars
  };
  NodePrintCallbackData postfixData = {
    .sink = stderr, 
    .c = '>'
  };
  nodeTraverse(diffTreeY, 
               .prefix  = nodePutcCallback,
               .infix   = nodePrintCallback,
               .postfix = nodePutcCallback,
               .prefixData  = &prefixData,
               .infixData   = &infixData,
               .postfixData = &postfixData);
  fprintf(stderr, "\n\n\n");
  prefixData.vars = ctx.vars;
  nodeTraverse(diffTreeY, 
               .prefix  = nodePutcAndPrintCallback,
               .postfix = nodePutcCallback,
               .prefixData  = &prefixData,
               .postfixData = &postfixData);

  treeDestroy(tree, true);
  nodeDestroy(diffTreeX, true);
  contextDestroy(&ctx);
  closeHtmlLogFile(log);
}
