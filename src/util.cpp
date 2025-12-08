#include "util.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static const size_t MAX_TIMESTAMPED_FILE_PATH_LENGTH = 128;

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
