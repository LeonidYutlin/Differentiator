#include "error.h"
#include "../misc/util.h"

static const ErrorInfo ERRORS[] = {
  #define X(enm, mod, shDescr, descr)                                            \
    [enm] = {.error = enm,                                                       \
             .module = mod,                                                      \
             .str = #enm,                                                        \
             .shortDesc = shDescr,                                               \
             .desc = descr}, 

  UNITED_ERROR_LIST()
  #undef X
};

static const ErrorModuleInfo ERROR_MODULES[] = {
  #define X(enm, shDescr, descr)                                                 \
    [enm] = {.module = enm,                                                      \
             .str = #enm,                                                        \
             .shortDesc = shDescr,                                               \
             .desc = descr},                                                     
  
  ERROR_MODULE_LIST()
  #undef X
};

const size_t ERRORS_SIZE        = sizer(ERRORS);
const size_t ERROR_MODULES_SIZE = sizer(ERROR_MODULES);

ErrorInfo parseError(Error e) {
  return (e < 0 || (size_t)e >= ERRORS_SIZE)
             ? (ErrorInfo){.error = e} //other fields are default, aka "UnknownError"
             : ERRORS[e];
}

ErrorModuleInfo parseErrorModule(ErrorModule m) {
  return (m < 0 || (size_t)m >= ERRORS_SIZE)
             ? (ErrorModuleInfo){.module = m} //other fields are default, aka "UnknownModule"
             : ERROR_MODULES[m];
}
