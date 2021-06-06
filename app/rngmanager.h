#ifndef RNGMANAGER_H
#define RNGMANAGER_H

#include <Windows.h>
#include <QString>
#include <QStringList>

class RngManager
{
public:
    RngManager();
    RngManager(const QString &alg_id);
    ~RngManager();

public:
    QStringList enumAlgorithms() const;
    void generateRandom(uchar* buf, const ulong size) const;

private:
    BCRYPT_ALG_HANDLE hAlg_ = nullptr;
    BCRYPT_HASH_HANDLE hHash_ = nullptr;
};

#endif // RNGMANAGER_H
