#ifndef HASHMANAGER_H
#define HASHMANAGER_H

#include <Windows.h>
#include <QString>
#include <QStringList>

class HashManager
{
public:
    HashManager();
    HashManager(const QString& alg_id);
    ~HashManager();

public:
    QStringList enumAlgorithms() const;
    unsigned long getHashSize() const;
    void createHash();
    void stepHash(uchar* data, ulong size) const;
    void finishHash(uchar *result, ulong size) const;
    void destroyHash();

private:
    BCRYPT_ALG_HANDLE hAlg_ = nullptr;
    BCRYPT_HASH_HANDLE hHash_ = nullptr;
};

#endif // HASHMANAGER_H
