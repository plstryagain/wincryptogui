#include "hashmanager.h"
#include "cngexceptions.h"

#include <QDebug>

#include <memory>

HashManager::HashManager()
{

}

HashManager::HashManager(const QString &alg_id)
{
    auto buf = std::make_unique<wchar_t[]>(static_cast<size_t>(alg_id.size()) + 1);
    int len = alg_id.toWCharArray(buf.get());
    if (!len) {
        throw CngExceptions(ERROR_BAD_LENGTH, "Failed to convert QString to wchar array");
    }
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg_, buf.get(), nullptr, 0);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, std::string("Failed to open provider for algorithm " + alg_id.toUtf8()));
    }
}

HashManager::~HashManager()
{
    if (hHash_) {
        BCryptDestroyHash(hHash_);
    }
    if (hAlg_) {
        BCryptCloseAlgorithmProvider(hAlg_, 0);
    }
}

QStringList HashManager::enumAlgorithms() const
{
    ULONG alg_count = 0;
    BCRYPT_ALGORITHM_IDENTIFIER* algs_id = nullptr;
    NTSTATUS status = BCryptEnumAlgorithms(BCRYPT_HASH_OPERATION, &alg_count, &algs_id, 0);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, "Failed to enum hash algorithms");
    }
    QStringList alg_id_list;
    for (ULONG i = 0; i < alg_count; ++i) {
        alg_id_list.append(QString::fromWCharArray(algs_id[i].pszName));
    }
    if (algs_id) {
        BCryptFreeBuffer(algs_id);
    }
    return alg_id_list;
}

unsigned long HashManager::getHashSize() const
{
    unsigned long hash_size = 0;
    unsigned long res = 0;
    NTSTATUS status = BCryptGetProperty(hAlg_, BCRYPT_HASH_LENGTH, reinterpret_cast<uchar*>(&hash_size), sizeof(hash_size), &res, 0);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, "Failed to hash size");
    }
    return hash_size;
}

void HashManager::createHash()
{
    NTSTATUS status = BCryptCreateHash(hAlg_, &hHash_, nullptr, 0, nullptr, 0, 0);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, "Failed to create hash");
    }
}

void HashManager::stepHash(uchar* data, ulong size) const
{
    NTSTATUS status = BCryptHashData(hHash_, data, size, 0);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, "Failed to hash data");
    }
}

void HashManager::finishHash(uchar* result, ulong size) const
{
    NTSTATUS status = BCryptFinishHash(hHash_, result, size, 0);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, "Failed to finish hash");
    }
}

void HashManager::destroyHash()
{
    NTSTATUS status = BCryptDestroyHash(hHash_);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, "Failed to destroy hash");
    }
    hHash_ = nullptr;
}
