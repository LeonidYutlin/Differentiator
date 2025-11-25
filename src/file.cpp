#include <sys/stat.h>
#include <stdlib.h>

#include "file.h"

int readBufferFromFile(FILE* file,
                       char** bufferPtr, size_t* trueBufferSizePtr) {
    if (!file ||
        !bufferPtr ||
        !trueBufferSizePtr)
        return 1;

    struct stat fileStats = {0};
    fstat(file->_fileno, &fileStats);
    size_t bufferSize = (size_t) fileStats.st_size;
    //fprintf(stderr, "calling calloc with this being the size: %lu\n", bufferSize);
    char* buffer = (char*) calloc(bufferSize, sizeof(char));
    if (!buffer)
        return 2;

    //fprintf(stderr, "this is buffer before read: %s\n", buffer);
    bufferSize = fread(buffer, sizeof(char), bufferSize, file);
    //fprintf(stderr, "this is what i have read: %10s\n", buffer);

    *trueBufferSizePtr = bufferSize;
    *bufferPtr = buffer;

    return 0;
}
