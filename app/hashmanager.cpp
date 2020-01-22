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

NTSTATUS HashManager::enumAlgorithms(QStringList& alg_id_list) const
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

unsigned long HashManager::getHashSize() const
{
    unsigned long hash_size = 0;
    unsigned long res = 0;
    BCryptGetProperty(hAlg_, BCRYPT_HASH_LENGTH, reinterpret_cast<uchar*>(&hash_size), sizeof(hash_size), &res, 0);
    return hash_size;
}

NTSTATUS HashManager::createHash()
{
    return BCryptCreateHash(hAlg_, &hHash_, nullptr, 0, nullptr, 0, 0);
}

NTSTATUS HashManager::stepHash(uchar* data, ulong size) const
{
    return BCryptHashData(hHash_, data, size, 0);
}

NTSTATUS HashManager::finishHash(uchar* result, ulong size) const
{
    return BCryptFinishHash(hHash_, result, size, 0);
}

NTSTATUS HashManager::destroyHash()
{
    return BCryptDestroyHash(hHash_);
}
