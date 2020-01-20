#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QHash>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

signals:
    void notifyWorkerFinished();
    void notifyWorkerHashAlsEnumComplete(int err, QStringList alg_id_list);

public:
    void setParams(const QHash<QString, QString>& params);
    void startEnumHashAlgorithms();
    void startCalculateHash();

private:
    QHash<QString, QString> params_;
};

#endif // WORKER_H
