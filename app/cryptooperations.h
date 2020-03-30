#ifndef CRYPTOOPERATIONS_H
#define CRYPTOOPERATIONS_H

#include <QString>

class CryptoOperations
{
public:
    CryptoOperations();

public:
    int enumAlgorithms(const QString& type, QStringList& alg_id_list);
    int encrypt(const QByteArray& plain_text, const QString& pass, const QString& cipher_alg_id,
                const QString& chaining_mode, QByteArray& cipher_text, QByteArray& salt, QByteArray& iv);
    int decrypt(const QByteArray& cipher_text, const QString& pass, const QString& cipher_alg_id,
                const QString& chaining_mode, QByteArray iv, const int iteration_count, QByteArray salt,
                QByteArray& plain_text);
};

#endif // CRYPTOOPERATIONS_H
