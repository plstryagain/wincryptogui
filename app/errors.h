#ifndef ERRORS_H
#define ERRORS_H

enum ERRORS : int {
    SUCCESS = 0,
    UNKNOWN_ERROR,

    INVALID_PARAMETERS = 10,
    INVALID_OUT_FORM,
    UNSUPPORTED_ALGORITHM_TYPE,
    FAILED_TO_GET_HASH_SIZE = 20,
    FAILED_TO_OPEN_FILE,
    FAILED_TO_READ_FILE

};

class Errors {
public:
    static int ntstatusToErrorCode(const long ntstatus);
};

#endif // ERRORS_H
