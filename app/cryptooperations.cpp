#include "cryptooperations.h"
#include "errors.h"
#include "hashmanager.h"
#include "rngmanager.h"
#include "ciphermanager.h"
#include "cngexceptions.h"

#include <QVector>
#include <QFile>
#include <QDebug>

CryptoOperations::CryptoOperations()
{

}

QStringList CryptoOperations::enumAlgorithms2(const QString &type)
{
    if (type == "hash") {
        return HashManager().enumAlgorithms();
    } else if (type == "rng") {
        return RngManager().enumAlgorithms();
    } else if (type == "cipher") {
        return QStringList();
    } else {
        throw CngExceptions(NTE_BAD_ALGID, "Unsupproted algorithm");
    }
}

int CryptoOperations::enumAlgorithms(const QString &type, QStringList &alg_id_list)
{
    NTSTATUS status = ERROR_SUCCESS;
    if (type == "hash") {
        alg_id_list = HashManager().enumAlgorithms();
    } else if (type == "rng") {
        alg_id_list = RngManager().enumAlgorithms();
    } else if (type == "cipher") {
        status = CipherManager().enumAlgorithms(alg_id_list);
    } else {
        return ERRORS::UNSUPPORTED_ALGORITHM_TYPE;
    }
    return Errors::ntstatusToErrorCode(status);
}

void CryptoOperations::generateRandomBytes(const QString& alg_id, const int size, const QString& out_form, QString& bytes)
{
    if (alg_id.isEmpty() || out_form.isEmpty() || size < 1 || size > 99999) {
        throw CngExceptions(NTE_BAD_DATA, "Invalid arguments provided");
    }
    RngManager rng{ alg_id };
    QByteArray buf;
    buf.resize(size);
    rng.generateRandom(reinterpret_cast<uchar*>(buf.data()), static_cast<ulong>(size));
    if (out_form == "HEX") {
        bytes.append(buf.toHex());
    } else if (out_form == "Base64") {
        bytes.append(buf.toBase64());
    } else {
        throw CngExceptions(NTE_BAD_DATA, "Invalid arguments provided");
    }
}

QByteArray CryptoOperations::calcualteHash(const QByteArray& data_to_hash, const QString& alg_id)
{
    HashManager hm{ alg_id };
    hm.createHash();
    unsigned long hash_size = hm.getHashSize();
    hm.stepHash(const_cast<uchar*>(reinterpret_cast<const uchar*>(data_to_hash.data())),
                static_cast<ulong>(data_to_hash.size()));
    QByteArray hash;
    hash.resize(static_cast<int>(hash_size));
    hm.finishHash(reinterpret_cast<uchar*>(hash.data()), static_cast<ulong>(hash.size()));
    hm.destroyHash();
    return hash;
}


int CryptoOperations::encrypt(const QByteArray &plain_text, const QString &pass,
                              const QString &cipher_alg_id, const QString &chaining_mode, QByteArray &cipher_text,
                              QByteArray& salt, QByteArray &iv)
{
    /* generate random salt */
    RngManager rng("RNG");
    int salt_len = 8;
    salt.resize(salt_len);
    rng.generateRandom(reinterpret_cast<uchar*>(salt.data()), static_cast<ulong>(salt_len));
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
        rng.generateRandom(reinterpret_cast<uchar*>(iv.data()), block_len);
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

