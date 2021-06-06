#ifndef CNGEXCEPTIONS_H
#define CNGEXCEPTIONS_H

#include <Windows.h>
#include <stdexcept>
#include <string>

class CngExceptions : public std::runtime_error
{
public:
    CngExceptions(NTSTATUS status, const std::string& what);

public:
    NTSTATUS getStatus() const;

private:
    NTSTATUS status_;
};

#endif // CNGEXCEPTIONS_H
