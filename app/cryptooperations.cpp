#include "cryptooperations.h"
#include "errors.h"
#include "hashmanager.h"
#include "rngmanager.h"
#include "ciphermanager.h"

#include <QVector>
#include <QDebug>

CryptoOperations::CryptoOperations()
{

}

int CryptoOperations::enumAlgorithms(const QString &type, QStringList &alg_id_list)
{
    NTSTATUS status = ERROR_SUCCESS;
    if (type == "hash") {
        status = HashManager().enumAlgorithms(alg_id_list);
    } else if (type == "rng") {
        status = RngManager().enumAlgorithms(alg_id_list);
    } else if (type == "cipher") {
        status = CipherManager().enumAlgorithms(alg_id_list);
    } else {
        return ERRORS::UNSUPPORTED_ALGORITHM_TYPE;
    }
    return Errors::ntstatusToErrorCode(status);
}

int CryptoOperations::encrypt(const QByteArray &plain_text, const QString &pass,
                              const QString &cipher_alg_id, const QString &chaining_mode, QByteArray &cipher_text,
                              QByteArray& salt, QByteArray &iv)
{
    /* generate random salt */
    RngManager rng;
    long status = rng.init("RNG");
    if (status != ERROR_SUCCESS) {
        return Errors::ntstatusToErrorCode(status);
    }
    int salt_len = 8;
    salt.resize(salt_len);
    status = rng.generateRandom(reinterpret_cast<uchar*>(salt.data()), static_cast<ulong>(salt_len));
    if (status != ERROR_SUCCESS) {
        return Errors::ntstatusToErrorCode(status);
    }
    CipherManager ciph;
    status = ciph.init(cipher_alg_id);
    if (status != ERROR_SUCCESS) {
        return Errors::ntstatusToErrorCode(status);
    }
    QVector<uchar> derived_key(32);
    do {
        /* derive key */
        status = ciph.deriveKey("SHA256", reinterpret_cast<uchar*>(pass.toUtf8().data()),
                                static_cast<ulong>(pass.toUtf8().size()), reinterpret_cast<uchar*>(salt.data()),
                                static_cast<ulong>(salt_len), 10000, derived_key.data(), 32);
        if (status != ERROR_SUCCESS) {
            break;
        }
        /* create key object */
        status = ciph.createKeyObject(derived_key.data(), static_cast<ulong>(derived_key.size()));
        if (status != ERROR_SUCCESS) {
            break;
        }
        ulong block_len = 0;
        /* generate iv */
        status = ciph.getBlockLength(&block_len);
        if (status != ERROR_SUCCESS) {
            break;
        }
        iv.resize(static_cast<int>(block_len));
        status = rng.generateRandom(reinterpret_cast<uchar*>(iv.data()), block_len);
        if (status != ERROR_SUCCESS) {
            break;
        }
        /* set chaining mode */
        status = ciph.setChainingMode(chaining_mode);
        if (status != ERROR_SUCCESS) {
            break;
        }
        /* get cipher text length */
        ulong cipher_text_len = 0;
        status = ciph.encrypt(reinterpret_cast<uchar*>(const_cast<char*>(plain_text.data())),
                              static_cast<ulong>(plain_text.size()),
                              reinterpret_cast<uchar*>(iv.data()), static_cast<ulong>(iv.size()),
                              nullptr, &cipher_text_len);
        if (status != ERROR_SUCCESS) {
            break;
        }
        cipher_text.resize(static_cast<int>(cipher_text_len));
        /* BCryptEncrypt modify iv parameter, so nedd to make copy */
        QByteArray iv_copy = iv;
        /* now encrypt */
        status = ciph.encrypt(reinterpret_cast<uchar*>(const_cast<char*>(plain_text.data())),
                              static_cast<ulong>(plain_text.size()),
                              reinterpret_cast<uchar*>(iv_copy.data()), static_cast<ulong>(iv_copy.size()),
                              reinterpret_cast<uchar*>(cipher_text.data()), &cipher_text_len);
    } while (0);
    ciph.destroyKeyObject();
    /* clear encrypting key */
    SecureZeroMemory(derived_key.data(), derived_key.size());
    return Errors::ntstatusToErrorCode(status);
}

int CryptoOperations::decrypt(const QByteArray &cipher_text, const QString &pass, const QString &cipher_alg_id, const QString &chaining_mode, QByteArray iv, const int iteration_count, QByteArray salt, QByteArray &plain_text)
{
    CipherManager ciph;
    long status = ciph.init(cipher_alg_id);
    if (status != ERROR_SUCCESS) {
        return Errors::ntstatusToErrorCode(status);
    }
    QVector<uchar> derived_key(32);
    do {
        /* derive key */
        status = ciph.deriveKey("SHA256", reinterpret_cast<uchar*>(pass.toUtf8().data()),
                                static_cast<ulong>(pass.toUtf8().size()), reinterpret_cast<uchar*>(salt.data()),
                                static_cast<ulong>(salt.size()), static_cast<ulong>(iteration_count), derived_key.data(), 32);
        if (status != ERROR_SUCCESS) {
            break;
        }
        /* create key object */
        status = ciph.createKeyObject(derived_key.data(), static_cast<ulong>(derived_key.size()));
        if (status != ERROR_SUCCESS) {
            break;
        }
        /* set chaining mode */
        status = ciph.setChainingMode(chaining_mode);
        if (status != ERROR_SUCCESS) {
            break;
        }
        /* get expected plain text length */
        ulong plain_text_len = 0;
        status = ciph.decrypt(reinterpret_cast<uchar*>(const_cast<char*>(cipher_text.data())), static_cast<ulong>(cipher_text.size()),
                              reinterpret_cast<uchar*>(iv.data()), static_cast<ulong>(iv.size()),
                              nullptr, &plain_text_len);
        if (status != ERROR_SUCCESS) {
            break;
        }
        plain_text.resize(static_cast<int>(plain_text_len));
        /* decrypt */
        status = ciph.decrypt(reinterpret_cast<uchar*>(const_cast<char*>(cipher_text.data())), static_cast<ulong>(cipher_text.size()),
                              reinterpret_cast<uchar*>(iv.data()), static_cast<ulong>(iv.size()),
                              reinterpret_cast<uchar*>(plain_text.data()), &plain_text_len);
        /* actualize size */
        plain_text.resize(static_cast<int>(plain_text_len));

    } while (0);
    ciph.destroyKeyObject();
    /* clear encrypting key */
    SecureZeroMemory(derived_key.data(), derived_key.size());
    return Errors::ntstatusToErrorCode(status);
}
