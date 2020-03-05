#include "backend.h"
#include "worker.h"
#include "constants.h"
#include "cryptooperations.h"
#include "util.h"
#include "errors.h"

#include <QThread>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
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

void Backend::enumCipherAlgorithms()
{
    auto* thread = new QThread();
    connect(thread, &QThread::started, [&](){
        QStringList alg_id_list;
        int err = CryptoOperations().enumAlgorithms("cipher", alg_id_list);
        emit notifyCipherAlgsEnumCompleted(err, alg_id_list);
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Backend::encrypt(const QString &cipher_alg_id, const QString& pass, const QString &plain_text_file,
                      const QString& path_to_save)
{
    auto* thread = new QThread();
    connect(thread, &QThread::started, [&, cipher_alg_id, pass, plain_text_file](){
        Util util;
        QByteArray plain_text;
        /* QML FileDialog return path that begins with "file:///" */
        int err = util.readFile(QUrl(plain_text_file).toLocalFile(), plain_text);
        if (err != ERRORS::SUCCESS) {
            emit notifyEncrypted(err, "");
            return;
        }
        CryptoOperations cop;
        QByteArray cipher_text, salt;
        err = cop.encrypt(plain_text, pass, cipher_alg_id, cipher_text, salt);
        if (err != ERRORS::SUCCESS) {
            emit notifyEncrypted(err, "");
            return;
        }
        /* save to file in JSON:
         * {
         *  "iteration_count": number,
         *  "salt": "base64 string",
         *  "alg_id": "string",
         *  "cipher_text": "base64 string"
         * }
         */
        QJsonObject jobj;
        jobj[CIPHER_PARAM::ITERATION_COUNT] = 10000;
        jobj[CIPHER_PARAM::SALT] = QString(salt.toBase64());
        jobj[CIPHER_PARAM::ALG_ID] = cipher_alg_id;
        jobj[CIPHER_PARAM::CIPHER_TEXT] = QString(cipher_text.toBase64());
        QJsonDocument cipher_doc(jobj);
        QByteArray cipher_doc_data = cipher_doc.toJson();
        QFileInfo fi(plain_text_file);
        QString full_path = QUrl(path_to_save).toLocalFile() + "/" + fi.fileName() + ".enc";
        qDebug() << full_path;
        err = util.writeToFile(full_path, cipher_doc_data);
        emit notifyEncrypted(err, QString(cipher_doc_data));
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}
