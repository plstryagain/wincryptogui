#include "backend.h"
#include "worker.h"
#include "constants.h"

#include <QThread>
#include <QDebug>

Backend::Backend(QObject *parent) : QObject(parent)
{

}

void Backend::enumHashAlgorithms()
{
    auto thread = new QThread();
    auto worker = new Worker();
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::startEnumHashAlgorithms);
    connect(worker, &Worker::notifyWorkerFinished, thread, &QThread::quit);
    connect(worker, &Worker::notifyWorkerFinished, worker, &Worker::deleteLater);
    connect(worker, &Worker::notifyWorkerHashAlsEnumComplete, this, &Backend::notifyHashAlsEnumComplete);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Backend::calculateHash(const QString &path, const bool is_dir, const QString &alg_id)
{
    QHash<QString, QString> params;
    params[HASH_PARAM::PATH] = path;
    params[HASH_PARAM::IS_DIR] = is_dir ? "true" : "false";
    params[HASH_PARAM::ALG_ID] = alg_id;
    auto thread = new QThread();
    auto worker = new Worker();
    worker->setParams(params);
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::startCalculateHash);
    connect(worker, &Worker::notifyWorkerFinished, thread, &QThread::quit);
    connect(worker, &Worker::notifyWorkerFinished, worker, &Worker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}
