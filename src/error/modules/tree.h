#ifndef ERROR_MODULE_TREE
#define ERROR_MODULE_TREE

#define TREE_STATUS_LIST()                                     \
  X(UninitializedTree,                                         \
    Tree,                                                      \
    "Tree is uninitialized",                                   \
    "Tree is uninitialized, "                                  \
    "use nodeInit()/nodeDynamicInit() or tree- counterparts")  \
  X(DestroyedTree,                                             \
    Tree,                                                      \
    "Tree is destroyed",                                       \
    "Tree has been destroyed before, "                         \
    "use initializers to reinit it")                           \
  X(AttemptedReinitialization,                                 \
    Tree,                                                      \
    "Attempted to reinitialize a working tree",                \
    "Attempted to reinitialize a working tree, "               \
    "if you wish to reinit this tree, destroy it and init it") \
  X(FailReadNode,                                              \
    Tree,                                                      \
    "Failed to read node",                                     \
    "Failed to read node, due to incorrect syntax. "           \
    "See logs for more info")

#endif
