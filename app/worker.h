#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QHash>

#include "hashmanager.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

signals:
    void notifyWorkerFinished();
    void notifyWorkerHashAlgsEnumComplete(int err, QStringList alg_id_list);
    void notifyWorkerOneHashCalculated(int err, QString alg_id, QString file_name, QString hash);

public:
    void setParams(const QHash<QString, QString>& params);
    void startEnumHashAlgorithms();
    void startCalculateHash();

private:
    long calcualteHash(HashManager &hm, const QString& file_path, QByteArray& hash);

private:
    QHash<QString, QString> params_;
};

#endif // WORKER_H
