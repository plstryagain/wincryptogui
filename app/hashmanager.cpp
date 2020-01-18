#include "hashmanager.h"

#include <QDebug>

HashManager::HashManager()
{

}

HashManager::~HashManager()
{

}

NTSTATUS HashManager::enumAlgorithms(QStringList& alg_id_list)
{
    ULONG alg_count = 0;
    BCRYPT_ALGORITHM_IDENTIFIER* algs_id = nullptr;
    NTSTATUS status = BCryptEnumAlgorithms(BCRYPT_HASH_OPERATION, &alg_count, &algs_id, 0);
    if (status != ERROR_SUCCESS) {
        return status;
    }

    for (ULONG i = 0; i < alg_count; ++i) {
        alg_id_list.append(QString::fromWCharArray(algs_id[i].pszName));
    }

    if (algs_id) {
        BCryptFreeBuffer(algs_id);
    }

    return ERROR_SUCCESS;
}
