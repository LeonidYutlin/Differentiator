#include "util.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>

static const size_t MAX_TIMESTAMPED_FILE_PATH_LENGTH = 128;
static const double DOUBLE_COMPARISON_PRECISION = 1e-9;

bool doubleEqual(double a, double b) {
    return fabs(a - b) < DOUBLE_COMPARISON_PRECISION;
}

char* getTimestampedString(const char* prefix, const char* suffix, uint count) {
    time_t timeAbs = time(NULL);
    tm* localTime = localtime(&timeAbs);
    char* name    = (char*)calloc(MAX_TIMESTAMPED_FILE_PATH_LENGTH, sizeof(char));
    char* pattern = (char*)calloc(MAX_TIMESTAMPED_FILE_PATH_LENGTH, sizeof(char));
    if (!pattern || !name) {
        free(name); free(pattern);
        return NULL;
    }

    if (count) {
        snprintf(pattern, MAX_TIMESTAMPED_FILE_PATH_LENGTH,
                 "%s%%d-%%m-%%Y-%%H:%%M:%%S-%u%s",
                 prefix,
                 count,
                 suffix);
    } else {
        snprintf(pattern, MAX_TIMESTAMPED_FILE_PATH_LENGTH,
                 "%s%%d-%%m-%%Y-%%H:%%M:%%S%s",
                 prefix,
                 suffix);
    }
    if (!strftime(name, MAX_TIMESTAMPED_FILE_PATH_LENGTH, pattern, localTime)){
        free(name); free(pattern);
        return NULL;
    }
    free(pattern);
    return name;
}

Error readBufferFromFile(FILE* file,
                         char** bufferPtr, size_t* trueBufferSizePtr) {
    if (!file ||
        !bufferPtr ||
        !trueBufferSizePtr)
        return InvalidParameters;

    struct stat fileStats = {0};
    fstat(file->_fileno, &fileStats);
    size_t bufferSize = (size_t) fileStats.st_size;
    char* buffer = (char*) calloc(bufferSize, sizeof(char));
    if (!buffer)
        return FailMemoryAllocation;

    bufferSize = fread(buffer, sizeof(char), bufferSize, file);

    *trueBufferSizePtr = bufferSize;
    *bufferPtr = buffer;

    return OK;
}
