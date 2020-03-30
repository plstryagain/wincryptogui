#ifndef CIPHERMANAGER_H
#define CIPHERMANAGER_H

#include <Windows.h>
#include <QString>
#include <QStringList>

class CipherManager
{
public:
    CipherManager();
    ~CipherManager();

public:
    NTSTATUS enumAlgorithms(QStringList &alg_id_list) const;
    NTSTATUS init(const QString& alg_id);
    NTSTATUS deriveKey(const QString& prf_alg_id, uchar* pass, ulong pass_len, uchar* salt, ulong salt_len,
                       const ulong iteration, uchar* derived_key, ulong derived_key_len) const;
    NTSTATUS createKeyObject(uchar* secret, ulong secret_len);
    NTSTATUS destroyKeyObject();
    NTSTATUS getBlockLength(ulong* block_len);
    NTSTATUS setKeyLength(ulong key_len);
    NTSTATUS setChainingMode(const QString& mode);
    NTSTATUS encrypt(uchar* plain_text, ulong plain_text_len,
                     uchar* iv, ulong iv_len, uchar* cipher_text, ulong *cipher_text_len);
    NTSTATUS decrypt(uchar* cipher_text, ulong cipher_text_len, uchar* iv, ulong iv_len,
                     uchar* plain_text, ulong* plain_text_len);

private:
    BCRYPT_ALG_HANDLE hAlg_ = nullptr;
    BCRYPT_KEY_HANDLE hKey_ = nullptr;
};

#endif // CIPHERMANAGER_H
