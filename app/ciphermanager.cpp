#include "ciphermanager.h"

CipherManager::CipherManager()
{

}

CipherManager::~CipherManager()
{
    if (hKey_) {
        BCryptDestroyKey(hKey_);
    }
    if (hAlg_) {
        BCryptCloseAlgorithmProvider(hAlg_, 0);
    }
}

NTSTATUS CipherManager::enumAlgorithms(QStringList &alg_id_list) const
{
    ULONG alg_count = 0;
    BCRYPT_ALGORITHM_IDENTIFIER* algs_id = nullptr;
    NTSTATUS status = BCryptEnumAlgorithms(BCRYPT_CIPHER_OPERATION, &alg_count, &algs_id, 0);
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

NTSTATUS CipherManager::init(const QString &alg_id)
{
    auto buf = std::make_unique<wchar_t[]>(static_cast<size_t>(alg_id.size()) + 1);
    int len = alg_id.toWCharArray(buf.get());
    if (!len) {
        return ERROR_BAD_LENGTH;
    }
    return BCryptOpenAlgorithmProvider(&hAlg_, buf.get(), nullptr, 0);
}

NTSTATUS CipherManager::deriveKey(const QString &prf_alg_id, uchar *pass, ulong pass_len, uchar *salt,
                                  ulong salt_len, const ulong iteration, uchar *derived_key, ulong derived_key_len) const
{
    BCRYPT_ALG_HANDLE hPrf = nullptr;
    NTSTATUS status = ERROR_SUCCESS;
    auto buf = std::make_unique<wchar_t[]>(static_cast<size_t>(prf_alg_id.size()) + 1);
    int len = prf_alg_id.toWCharArray(buf.get());
    if (!len) {
        return ERROR_BAD_LENGTH;
    }
    do {
        status = BCryptOpenAlgorithmProvider(&hPrf, buf.get(), nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG);
        if (status != ERROR_SUCCESS) {
            break;
        }
        status = BCryptDeriveKeyPBKDF2(hPrf, pass, pass_len, salt, salt_len, iteration, derived_key, derived_key_len, 0);
    } while (0);
    if (hPrf) {
        BCryptCloseAlgorithmProvider(hPrf, 0);
    }
    return status;
}

NTSTATUS CipherManager::createKeyObject(uchar *secret, ulong secret_len)
{
    return BCryptGenerateSymmetricKey(hAlg_, &hKey_, nullptr, 0, secret, secret_len, 0);
}

NTSTATUS CipherManager::destroyKeyObject()
{
    if (hKey_) {
        return BCryptDestroyKey(hKey_);
    }
    return ERROR_SUCCESS;
}

NTSTATUS CipherManager::getBlockLength(ulong *block_len)
{
    ulong res = 0;
    return BCryptGetProperty(hAlg_, BCRYPT_BLOCK_LENGTH, reinterpret_cast<uchar*>(block_len), sizeof(ulong), &res, 0);
}

NTSTATUS CipherManager::setKeyLength(ulong key_len)
{
    return BCryptSetProperty(hKey_, BCRYPT_KEY_LENGTH, reinterpret_cast<uchar*>(&key_len), sizeof(key_len), 0);
}

NTSTATUS CipherManager::setChainingMode(const QString &mode)
{
    std::wstring wmode;
    if (mode == "ecb") {
        wmode = BCRYPT_CHAIN_MODE_ECB;
    } else if (mode == "cbc") {
        wmode = BCRYPT_CHAIN_MODE_CBC;
    } else if (mode == "cfb") {
        wmode = BCRYPT_CHAIN_MODE_CFB;
    } else if (mode == "ccm") {
        wmode = BCRYPT_CHAIN_MODE_CCM;
    } else if (mode == "gcm") {
        wmode = BCRYPT_CHAIN_MODE_GCM;
    } else {
        return ERROR_NOT_SUPPORTED;
    }
    uchar* ptr = reinterpret_cast<uchar*>(const_cast<wchar_t*>(wmode.data()));
    ulong size = static_cast<ulong>(sizeof(wchar_t) * (wmode.size() + 1));
    return BCryptSetProperty(hAlg_, BCRYPT_CHAINING_MODE, ptr, size, 0);
}

NTSTATUS CipherManager::encrypt(uchar *plain_text, ulong plain_text_len, uchar *iv, ulong iv_len, uchar *cipher_text, ulong* cipher_text_len)
{
    return BCryptEncrypt(hKey_, plain_text, plain_text_len, nullptr, iv, iv_len, cipher_text, *cipher_text_len, cipher_text_len, BCRYPT_BLOCK_PADDING);
}
