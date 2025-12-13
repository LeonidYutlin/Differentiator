#ifndef ERROR_H
#define ERROR_H

typedef int Error; //Ментору: может лучше Status?

const int NON_GENERIC_ERROR = 8;

enum GenericError {
    OK = 0,
    InvalidParameters,
    FailMemoryAllocation,
    FailMemoryReallocation,
};

#endif
