#include <Windows.h>
#include "errors.h"

int Errors::ntstatusToErrorCode(const long ntstatus)
{
    switch (ntstatus) {
    case ERROR_SUCCESS:
        return ERRORS::SUCCESS;
    default:
        return ERRORS::UNKNOWN_ERROR;
    }
}
