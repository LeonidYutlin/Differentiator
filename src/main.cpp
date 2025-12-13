#include "tree.h"
#include "node.h"
#include "dump.h"
#include "diff.h"
#include "calc.h"
#include "output.h"

//TODO adapt eval for trees
//TODO multibyte var names (requires a different memorization system)
//TODO unions
//TODO calculation with given var values
//TODO differentiation
//TODO Taylor Series
//TODO partial derivative
//TODO total derivative
int main() {
    const char* TESTS[] = {
        "1",
        "10+3",
        "10*7",
        "800-40*20",
        "400/20*20-400",
        "(30-5)/5",
        "10*7-(5+6*7-(10-40*10))",
    };
    size_t testN = sizeof(TESTS) / sizeof(TESTS[0]);
    for (size_t i = 0; i < testN; i++) {
        printf("TEST[%lu/%lu]: \"%s\" = %lg\n",
               i + 1, testN, TESTS[i], evaluate(TESTS[i]));
    }


    FILE* log = initHtmlLogFile();
    FILE* tex = initTexLogFile();

    FILE* expr = fopen(".test/expr3.txt", "r");
    TreeRoot* tree = treeRead(expr);
    if (!tree) {
        fprintf(stderr, "IT IS NULL\n");
        return 1;
    }
    fclose(expr);
    treeDump(log, tree, "<b3>Read tree</b3>");
    treeToTex(tex, tree);
    TreeNode* diffTreeX = differentiate(tree->rootNode, 'x', tex);
    // TreeNode* diffTreeY = differentiate(tree->rootNode, 'y', tex);
    // TreeNode* diffTreeZ = differentiate(tree->rootNode, 'z', tex);
    nodeDump(log, diffTreeX, "<b3> tree after diff </b3>");
    nodeToTex(tex, diffTreeX);
    closeTexLogFile(tex);
    fclose(log);

    treeDestroy(tree, true);
    nodeDestroy(diffTreeX, true);
    // nodeDestroy(diffTreeY, true);
    // nodeDestroy(diffTreeZ, true);
}

