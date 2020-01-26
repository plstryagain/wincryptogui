#ifndef ERRORS_H
#define ERRORS_H

enum ERRORS : int {
    SUCCESS = 0,

    INVALID_PARAMETERS = 10,
    FAILED_TO_GET_HASH_SIZE = 20,
    FAILED_TO_OPEN_FILE
};

#endif // ERRORS_H
