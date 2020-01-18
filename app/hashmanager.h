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

private:
};

#endif // HASHMANAGER_H
