#ifndef RNGMANAGER_H
#define RNGMANAGER_H

#include <Windows.h>
#include <QString>
#include <QStringList>

class RngManager
{
public:
    RngManager();
    ~RngManager();

public:
    NTSTATUS enumAlgorithms(QStringList &alg_id_list) const;
    NTSTATUS init(const QString& alg_id);
    NTSTATUS generateRandom(uchar* buf, const ulong size) const;

private:
    BCRYPT_ALG_HANDLE hAlg_ = nullptr;
    BCRYPT_HASH_HANDLE hHash_ = nullptr;
};

#endif // RNGMANAGER_H
