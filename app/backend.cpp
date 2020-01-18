#include "backend.h"
#include "worker.h"

#include <QThread>

Backend::Backend(QObject *parent) : QObject(parent)
{

}

void Backend::enumHashAlgorithms()
{
    auto thread = new QThread();
    auto worker = new Worker();
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::enumHashAlgorithms);
    connect(worker, &Worker::notifyWorkerFinished, thread, &QThread::quit);
    connect(worker, &Worker::notifyWorkerFinished, worker, &Worker::deleteLater);
    connect(worker, &Worker::notifyHashAlsEnumComplete, this, &Backend::notifyHashAlsEnumComplete);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}
