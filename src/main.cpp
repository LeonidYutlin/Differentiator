#include "treedef.h"
#include "nodedef.h"
#include "dump.h"
#include "diff.h"

//TODO multibyte var names (requires a different memorization system)
//TODO treeInit without node params so it leaves it empty, much easier to use if you want to append root to smth
//TODO calculation with given var values
//TODO differentiation
//TODO Taylor Series
//TODO partial derivative
//TODO total derivative
int main() {
    FILE* log = initHtmlLogFile();
    FILE* tex = initTexLogFile();

    FILE* expr = fopen(".test/expr3.txt", "r");
    TreeRoot* tree = treeRead(expr);
    if (!tree)
        fprintf(stderr, "IT IS NULL\n");
    fclose(expr);
    treeDump(log, tree, "<b3>Read tree</b3>");
    treeTexDump(tex, tree);
    TreeNode* diffTree = differentiate(tree->rootNode, 'x', tex);
    nodeTexDump(tex, diffTree);
    nodeDump(log, diffTree, "<b3> tree after diff </b3>");
    closeTexLogFile(tex);

    treeDestroy(tree, true);
    nodeDestroy(diffTree, true);
}

