#ifndef ERROR_MODULE_GENERIC
#define ERROR_MODULE_GENERIC

#define GENERIC_ERROR_LIST()                                                       \
  X(OK,                                                                            \
    Generic,                                                                       \
    "Success",                                                                     \
    "No errors occured")                                                           \
  X(InvalidParameters,                                                             \
    Generic,                                                                       \
    "Invalid parameters",                                                          \
    "Invalid parameters were provided for a function call")                        \
  X(FailMemoryAllocation,                                                          \
    Generic,                                                                       \
    "Failed memory allocation",                                                    \
    "Failed to allocate memory (likely in m(c)alloc())")                           \
  X(FailMemoryReallocation,                                                        \
    Generic,                                                                       \
    "Failed memory reallocation",                                                  \
    "Failed to reallocate memory (realloc()). "                                    \
    "Allocated memory is still stored where it was before attempted reallocation") \
  X(FailFileOpen,                                                                  \
    Generic,                                                                       \
    "Failed to open file",                                                         \
    "Failed to open file. See perror() for more info")

#endif
