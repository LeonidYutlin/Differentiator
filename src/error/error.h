#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>
#include <stdio.h>
#include "error/modules/generic.h"
#include "error/modules/tree.h"
#include "error/modules/variables.h"

typedef int Error;

//TODO: whether the error is saved in dt, whether verify passes with it (is it soft, fatal or ...)
//INFO: Use this one when you want to iterate through every single one, since it keeps the same order
#define UNITED_ERROR_LIST() \
  GENERIC_ERROR_LIST()      \
  TREE_STATUS_LIST()        \
  VARIABLES_STATUS_LIST()

#define ERROR_MODULE_LIST()                               \
  X(GenericError,                                         \
    "Generic Errors",                                     \
    "Common errors across the whole project")             \
  X(TreeError,                                            \
    "Tree/Node Errors",                                   \
    "Errors related to TreeRoot/TreeNode datastructures") \
  X(VariablesError,                                       \
    "Variables Errors",                                   \
    "Errors related to dyn array of variables - Variables")

enum ErrorModule {
  #define X(enm, ...) enm,
  ERROR_MODULE_LIST()
  #undef X
};
enum ErrorEnum {
  #define X(enm, ...) enm,
  UNITED_ERROR_LIST()
  #undef X
};

extern const size_t ERROR_MODULES_SIZE;
extern const size_t ERRORS_SIZE;

struct ErrorModuleInfo {
  ErrorModule module;
  const char* str       = "UnknownModule";
  const char* shortDesc = "Unknown module";
  const char* desc      = "A module with such value of enum is not present in ERROR_MODULES[]";
};
struct ErrorInfo {
  Error error = -1;
  ErrorModule module = GenericError;
  const char* str       = "UnknownError";
  const char* shortDesc = "Unknown error";
  const char* desc      = "An error with such error code is not present in ERRORS[]";
};

const ErrorModuleInfo* parseErrorModule(ErrorModule module);
const ErrorInfo* parseError(Error error);

void prettyError(FILE* sink, Error error, const char* filaname, int line);
#define prettyError(sink, error) prettyError(sink, error, __FILE__, __LINE__)

Error dumpErrors(FILE* file);

#endif
