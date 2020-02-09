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
    connect(worker, &Worker::notifyWorkerHashAlgsEnumComplete, this, &Backend::notifyHashAlsEnumComplete);
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
    connect(worker, &Worker::notifyWorkerOneHashCalculated, this, &Backend::notifyOneHashCalculated);
    connect(worker, &Worker::notifyWorkerFinished, thread, &QThread::quit);
    connect(worker, &Worker::notifyWorkerFinished, worker, &Worker::deleteLater);
    connect(worker, &Worker::notifyWorkerFinished, this, &Backend::notifyOperationFinished);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Backend::compareFiles(const QString &first_file, const QString &second_file, const QString &alg_id)
{
    QHash<QString, QString> params;
    params[HASH_PARAM::ALG_ID] = alg_id;
    params[HASH_PARAM::FIRST_FILE] = first_file;
    params[HASH_PARAM::SECOND_FILE] = second_file;
    auto thread = new QThread();
    auto worker = new Worker();
    worker->setParams(params);
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::startCompareFiles);
    connect(worker, &Worker::notifyWorkerFilesCompared, this, &Backend::notifyFilesCompared);
    connect(worker, &Worker::notifyWorkerFinished, thread, &QThread::quit);
    connect(worker, &Worker::notifyWorkerFinished, worker, &Worker::deleteLater);
    connect(worker, &Worker::notifyWorkerFinished, this, &Backend::notifyOperationFinished);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Backend::enumRngAlgorithms()
{
    auto thread = new QThread();
    auto worker = new Worker();
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::startEnumRngAlgorithms);
    connect(worker, &Worker::notifyWorkerFinished, thread, &QThread::quit);
    connect(worker, &Worker::notifyWorkerFinished, worker, &Worker::deleteLater);
    connect(worker, &Worker::notifyWorkerRngAlgsEnumComplete, this, &Backend::notifyRngAlgsEnumComplete);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Backend::generateRandom(const QString &alg_id, const QString& size, const QString &out_form)
{
    QHash<QString, QString> params;
    params[RNG_PARAM::ALG_ID] = alg_id;
    params[RNG_PARAM::SIZE] = size;
    params[RNG_PARAM::OUT_FORM] = out_form;
    auto thread = new QThread();
    auto worker = new Worker();
    worker->setParams(params);
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::startGenerateRandomBytes);
    connect(worker, &Worker::notifyWorkerRandomBytesGenerated, this, &Backend::notifyRandomBytesGenerated);
    connect(worker, &Worker::notifyWorkerFinished, thread, &QThread::quit);
    connect(worker, &Worker::notifyWorkerFinished, worker, &Worker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}
