#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include <Windows.h>
#include <QString>
#include <QStringList>

class HashManager
{
public:
    HashManager();
    ~HashManager();

public:
    NTSTATUS enumAlgorithms(QStringList &alg_id_list) const;
    NTSTATUS init(const QString& alg_id);
    unsigned long getHashSize() const;
    NTSTATUS createHash();
    NTSTATUS stepHash(uchar* data, ulong size) const;
    NTSTATUS finishHash(uchar *result, ulong size) const;
    NTSTATUS destroyHash();

private:
    BCRYPT_ALG_HANDLE hAlg_ = nullptr;
    BCRYPT_HASH_HANDLE hHash_ = nullptr;
};

#endif // HASHMANAGER_H
