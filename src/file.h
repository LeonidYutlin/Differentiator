#ifndef FILE_H
#define FILE_H

#include <stdio.h>

int readBufferFromFile(FILE* file,
                       char** bufferPtr, size_t* trueBufferSizePtr);

#endif
