#include "calc.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>

static double getE(const char* buf, size_t* p);
static double getT(const char* buf, size_t* p);
static double getP(const char* buf, size_t* p);
static double getN(const char* buf, size_t* p);

#define SYNTAX_ERROR(commentary, expectedCharStr, pos) \
        { \
        fprintf(stderr, \
                "[ERROR]: Failed to read given mathematical expression\n" \
                "At position %lu\n" \
                "%s\n" \
                "Expected char: %s\n" \
                "String snippet:\n" \
                "%.10s...\n" \
                "^\n", \
                pos, \
                commentary, \
                expectedCharStr, \
                buf + pos); \
        return NAN; \
        }

double evaluate(const char* buf) {
    size_t p = 0;
    double val = getE(buf, &p);
    if (buf[p] != '\0')
        SYNTAX_ERROR("Illegal character at the end of given expression", "NULL character ('\\0')", p);
    p++;
    return val;
}

static double getE(const char* buf, size_t* p) {
    double val = getT(buf, p);
    while (buf[*p] == '+' ||
           buf[*p] == '-') {
        char op = buf[*p];
        (*p)++;
        double val2 = getT(buf, p);
        if (op == '+')
            val += val2;
        else
            val -= val2;
    }
    return val;
}

static double getT(const char* buf, size_t* p) {
    double val = getP(buf, p);
    while (buf[*p] == '*' ||
           buf[*p] == '/') {
        char op = buf[*p];
        (*p)++;
        double val2 = getP(buf, p);
        if (op == '*')
            val *= val2;
        else
            val /= val2;
    }
    return val;
}

static double getP(const char* buf, size_t* p) {
    while (buf[*p] == '(') {
        (*p)++;
        double val = getE(buf, p);
        if (buf[*p] == ')')
            (*p)++;
        else
            SYNTAX_ERROR("Illegal character at the end of a primary expression", ")", *p);
        return val;
    }
    return getN(buf, p);
}


static double getN(const char* buf, size_t* p) {
    double val = 0;
    size_t oldP = *p;
    while (buf[*p] >= '0' &&
           buf[*p] <= '9') {
        val = val * 10 + (buf[*p] - '0');
        (*p)++;
    }
    if (oldP == *p)
        SYNTAX_ERROR("Illegal char at the start of a number", "[0-9]", *p);

    return val;
}

#undef SYNTAX_ERROR
