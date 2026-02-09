#include "ds/tree/tree.h"
#include "parse.h"
#include <ctype.h>

static TreeNode* getE(const char* buf, size_t* p);
static TreeNode* getT(const char* buf, size_t* p);
static TreeNode* getP(const char* buf, size_t* p);
static TreeNode* getN(const char* buf, size_t* p);

#define SYNTAX_ERROR(commentary, expectedCharStr, p)                \
  {                                                                 \
  fprintf(stderr,                                                   \
          "[ERROR]: Failed to read given mathematical expression\n" \
          "\tAt position %lu\n"                                     \
          "\t%s\n"                                                  \
          "\tExpected char: %s\n"                                   \
          "\tString snippet:\n"                                     \
          "\t%.10s...\n"                                            \
          "\t^\n",                                                  \
          p,                                                        \
          commentary,                                               \
          expectedCharStr,                                          \
          buf + p);                                                 \
  return NULL;                                                      \
  }

#define SKIP_WHITESPACE(a)    \
  while (isspace(buf[(a)])) { \
      fprintf(stderr, "Remaining string \"%s\"\n", buf + (a)); \
      (a)++;                  \
  }

TreeNode* parseFormula(const char* buf) {
  size_t p = 0;
  TreeNode* val = getE(buf, &p);
  SKIP_WHITESPACE(p);
  if (buf[p] != '\0')
    SYNTAX_ERROR("Illegal character at the end of given expression", "NULL character ('\\0')", p);
  p++;
  nodeFixParents(val);
  return val;
}

static TreeNode* getE(const char* buf, size_t* p) {
  SKIP_WHITESPACE(*p);
  TreeNode* val = getT(buf, p);
  while (buf[*p] == '+' ||
         buf[*p] == '-') {
    char op = buf[*p];
    (*p)++;
    TreeNode* val2 = getT(buf, p);
    if (op == '+')
      val = ADD_(val, val2);
    else
      val = SUB_(val, val2);
  }
  SKIP_WHITESPACE(*p);
  return val;
}

static TreeNode* getT(const char* buf, size_t* p) {
  SKIP_WHITESPACE(*p);
  TreeNode* val = getP(buf, p);
  while (buf[*p] == '*' ||
         buf[*p] == '/') {
    char op = buf[*p];
    (*p)++;
    TreeNode* val2 = getP(buf, p);
    if (op == '*')
      val = MUL_(val, val2);
    else
      val = DIV_(val, val2);
  }
  SKIP_WHITESPACE(*p);
  return val;
}

static TreeNode* getP(const char* buf, size_t* p) {
  SKIP_WHITESPACE(*p);
  while (buf[*p] == '(') {
    (*p)++;
    SKIP_WHITESPACE(*p);
    TreeNode* val = getE(buf, p);
    SKIP_WHITESPACE(*p);
    if (buf[*p] == ')')
      (*p)++;
    else
      SYNTAX_ERROR("Illegal character at the end of a primary expression", ")", *p);
    return val;
  }
  SKIP_WHITESPACE(*p);
  return getN(buf, p);
}

static TreeNode* getN(const char* buf, size_t* p) {
  SKIP_WHITESPACE(*p);
  double val = 0;
  size_t oldP = *p;
  while (buf[*p] >= '0' &&
         buf[*p] <= '9') {
      val = val * 10 + (buf[*p] - '0');
      fprintf(stderr, "I have found %c at %zu\n", buf[*p], *p);
      (*p)++;
  }
  if (oldP == *p)
    SYNTAX_ERROR("Illegal char at the start of a number", "[0-9]", *p);
  fprintf(stderr, "I am at '%c', at %zu\n", buf[*p], *p);
  SKIP_WHITESPACE(*p);
  fprintf(stderr, "Done\n");
  return NUM_(val);
}

#undef SYNTAX_ERROR
#undef SKIP_WHITESPACE

