#include "rngmanager.h"
#include "cngexceptions.h"

RngManager::RngManager()
{

}

RngManager::RngManager(const QString &alg_id)
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

RngManager::~RngManager()
{
    if (hAlg_) {
        BCryptCloseAlgorithmProvider(hAlg_, 0);
    }
}

QStringList RngManager::enumAlgorithms() const
{
    QStringList alg_id_list;
    ULONG alg_count = 0;
    BCRYPT_ALGORITHM_IDENTIFIER* algs_id = nullptr;
    NTSTATUS status = BCryptEnumAlgorithms(BCRYPT_RNG_OPERATION, &alg_count, &algs_id, 0);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, "Failed to enum RNG algorithms");
    }

    for (ULONG i = 0; i < alg_count; ++i) {
        alg_id_list.append(QString::fromWCharArray(algs_id[i].pszName));
    }

    if (algs_id) {
        BCryptFreeBuffer(algs_id);
    }
    return alg_id_list;
}

void RngManager::generateRandom(uchar *buf, const ulong size) const
{
    NTSTATUS status = BCryptGenRandom(hAlg_, buf, size, 0);
    if (status != ERROR_SUCCESS) {
        throw CngExceptions(status, "BCryptGenRandom failed with error");
    }
}

