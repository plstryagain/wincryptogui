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
    NTSTATUS enumAlgorithms(QStringList &alg_id_list);
    NTSTATUS init(const QString& alg_id);

private:
    BCRYPT_ALG_HANDLE hAlg_ = nullptr;
};

#endif // HASHMANAGER_H
