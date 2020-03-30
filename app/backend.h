#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

signals:
    void notifyOperationFinished();
    void notifyHashAlsEnumComplete(int err, QStringList alg_id_list);
    void notifyOneHashCalculated(int err, QString alg_id, QString file_name, QString hash);
    void notifyFilesCompared(int err, QString first_hash, QString second_hash, bool is_equal);

    void notifyRngAlgsEnumComplete(int err, QStringList alg_id_list);
    void notifyRandomBytesGenerated(int err, QString bytes);

    void notifyCipherAlgsEnumCompleted(int err, QStringList alg_id_list);
    void notifyEncrypted(int err, QString cipher_text);
    void notifyDecrypted(int err, QString plain_text);

public slots:
    void enumHashAlgorithms();
    void calculateHash(const QString& path, const bool is_dir, const QString& alg_id);
    void compareFiles(const QString& first_file, const QString& second_file, const QString& alg_id);

    void enumRngAlgorithms();
    void generateRandom(const QString& alg_id, const QString &size, const QString& out_form);

    void enumCipherAlgorithms();
    void encrypt(const QString& cipher_alg_id, QString pass, const QString& plain_text_file, const QString &path_to_save);
    void decrypt(QString pass, const QString& cipher_text_file, const QString &path_to_save);
};

#endif // BACKEND_H
