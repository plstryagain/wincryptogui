#include "worker.h"
#include "hashmanager.h"
#include "constants.h"

#include <QDebug>

Worker::Worker(QObject *parent) : QObject(parent)
{

}

Worker::~Worker()
{
    qDebug() << "WORKER DESTROYED";
}

void Worker::setParams(const QHash<QString, QString> &params)
{
    params_ = params;
}

void Worker::startEnumHashAlgorithms()
{
    QStringList alg_id_list;
    NTSTATUS status = HashManager().enumAlgorithms(alg_id_list);
    emit notifyWorkerHashAlsEnumComplete(static_cast<int>(status), alg_id_list);
    emit notifyWorkerFinished();
}

void Worker::startCalculateHash()
{
    QString path = params_[HASH_PARAM::PATH];
    QString alg_id = params_[HASH_PARAM::ALG_ID];
    QString is_dir = params_[HASH_PARAM::IS_DIR];
    if (path.isEmpty() || alg_id.isEmpty() || is_dir.isEmpty()) {
        qDebug() << "invalid arguments";
        emit notifyWorkerFinished();
        return;
    }
    HashManager hm;
    NTSTATUS status = hm.init(alg_id);
    emit notifyWorkerFinished();
}
