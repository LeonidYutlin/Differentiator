#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>
#include <stdio.h>
#include <math.h>
#include "../misc/error.h"

bool doubleEqual(double a, double b);

char* getTimestampedString(const char* prefix, const char* suffix, uint count = 0);

Error readBufferFromFile(FILE* file,
                         char** bufferPtr, size_t* trueBufferSizePtr);

#endif
