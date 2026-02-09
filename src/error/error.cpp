#include "error/error.h"
#include "misc/util.h"

#undef prettyError

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

const ErrorInfo* parseError(Error e) {
  return (e < 0 || (size_t)e >= ERRORS_SIZE)
         ? NULL
         : &ERRORS[e];
}

const ErrorModuleInfo* parseErrorModule(ErrorModule m) {
  return (m < 0 || (size_t)m >= ERROR_MODULES_SIZE)
         ? NULL
         : &ERROR_MODULES[m];
}

void prettyError(FILE* sink, Error error, const char* filename, int line) {
  if (!sink ||
      !filename)
    return;

  fprintf(sink, "%s:%d [ERROR]: ", filename, line);
  const ErrorInfo* err = parseError(error);
  if (!err) {
    ErrorInfo unknown = {0};
    fprintf(sink, 
            "%s (error code = %d): %s\n", 
            unknown.str,
            error,
            unknown.desc);
  } else {
    fprintf(sink, 
            "%s (error code = %d): %s\n", 
            err->str,
            error,
            err->desc);
  }
}

Error dumpErrors(FILE* file) {
  if (!file)
    return InvalidParameters;

  fprintf(file, 
          "Total modules: %zu\n"
          "Total errors : %zu\n",
          ERROR_MODULES_SIZE,
          ERRORS_SIZE);
  for (int i = 0, m = -1; (size_t)i < ERRORS_SIZE; i++) {
    const ErrorInfo* err = parseError(i);
    if (!err) continue;
    if (err->module != m) {
      m = err->module;
      const ErrorModuleInfo* mI = parseErrorModule((ErrorModule)m);
      if (!mI) continue;
      fprintf(file,
              "--ERROR MODULE #%d - %s--\n"
              "\tshort desc = %s\n"
              "\tdesc = %s\n",
              m, mI->str,
              mI->shortDesc,
              mI->desc);
    }
    fprintf(file, 
            "%d: %s\n"
            "\tshort desc = %s\n"
            "\tdesc = %s\n", 
            i, err->str, 
            err->shortDesc, 
            err->desc);
  }
  return OK;
}
