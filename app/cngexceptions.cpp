#include "cngexceptions.h"

CngExceptions::CngExceptions(NTSTATUS status, const std::string& what)
    : std::runtime_error(what), status_(status)
{

}

NTSTATUS CngExceptions::getStatus() const
{
    return status_;
}
