#include "worker.h"
#include "hashmanager.h"

#include <QDebug>

Worker::Worker(QObject *parent) : QObject(parent)
{

}

Worker::~Worker()
{
    qDebug() << "WORKER DESTROYED";
}

void Worker::enumHashAlgorithms()
{
    QStringList alg_id_list;
    NTSTATUS status = HashManager().enumAlgorithms(alg_id_list);
    emit notifyHashAlsEnumComplete(status, alg_id_list);
    emit notifyWorkerFinished();
}
