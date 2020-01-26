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

public slots:
    void enumHashAlgorithms();
    void calculateHash(const QString& path, const bool is_dir, const QString& alg_id);
    void compareFiles(const QString& first_file, const QString& second_file, const QString& alg_id);
};

#endif // BACKEND_H
