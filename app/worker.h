#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

signals:
    void notifyWorkerFinished();
    void notifyHashAlsEnumComplete(long err, QStringList alg_id_list);

public:
    void enumHashAlgorithms();
};

#endif // WORKER_H
