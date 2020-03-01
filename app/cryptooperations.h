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
                QByteArray& cipher_text, QByteArray& salt);
};

#endif // CRYPTOOPERATIONS_H
