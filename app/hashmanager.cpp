#include "hashmanager.h"

#include <QDebug>

#include <memory>

HashManager::HashManager()
{

}

HashManager::~HashManager()
{
    if (hAlg_) {
        BCryptCloseAlgorithmProvider(hAlg_, 0);
    }
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

NTSTATUS HashManager::init(const QString &alg_id)
{
    auto buf = std::make_unique<wchar_t[]>(static_cast<size_t>(alg_id.size()) + 1);
    int len = alg_id.toWCharArray(buf.get());
    if (!len) {
        return ERROR_BAD_LENGTH;
    }
    return BCryptOpenAlgorithmProvider(&hAlg_, buf.get(), nullptr, 0);
}
