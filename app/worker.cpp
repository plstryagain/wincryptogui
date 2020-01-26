#include "worker.h"
#include "constants.h"
#include "errors.h"

#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <QDirIterator>

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
    emit notifyWorkerHashAlgsEnumComplete(static_cast<int>(status), alg_id_list);
    emit notifyWorkerFinished();
}

void Worker::startCalculateHash()
{
    /* QML FileDialog return path that begins with "file:///" */
    QString path = QUrl(params_[HASH_PARAM::PATH]).toLocalFile();
    QString alg_id = params_[HASH_PARAM::ALG_ID];
    QString is_dir = params_[HASH_PARAM::IS_DIR];
    if (path.isEmpty() || alg_id.isEmpty() || is_dir.isEmpty()) {
        qDebug() << "invalid arguments";
        emit notifyWorkerOneHashCalculated(ERRORS::INVALID_PARAMETERS, alg_id, "", "");
        emit notifyWorkerFinished();
        return;
    }
    HashManager hm;
    NTSTATUS status = hm.init(alg_id);
    if (status != ERROR_SUCCESS) {
        emit notifyWorkerOneHashCalculated(static_cast<int>(status), alg_id, "", "");
        emit notifyWorkerFinished();
        return;
    }
    unsigned long hash_size = hm.getHashSize();
    if (hash_size == 0) {
        emit notifyWorkerOneHashCalculated(ERRORS::FAILED_TO_GET_HASH_SIZE, alg_id, "", "");
        emit notifyWorkerFinished();
        return;
    }
    if (is_dir == "false") {
        QByteArray hash;
        hash.resize(static_cast<int>(hash_size));
        status = calcualteHash(hm, path, hash);
        qDebug() << hash.toHex();
        emit notifyWorkerOneHashCalculated(static_cast<int>(status), alg_id, path, hash.toHex());
    } else {
        QDirIterator it(path, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QByteArray hash;
            hash.resize(static_cast<int>(hash_size));
            QString cur_path = it.next();
            status = calcualteHash(hm, cur_path, hash);
            qDebug() << hash.toHex();
            emit notifyWorkerOneHashCalculated(static_cast<int>(status), alg_id, cur_path, hash.toHex());
        }
    }
    emit notifyWorkerFinished();
}

void Worker::startCompareFiles()
{
    QString alg_id = params_[HASH_PARAM::ALG_ID];
    QString first_file = QUrl(params_[HASH_PARAM::FIRST_FILE]).toLocalFile();
    QString second_file = QUrl(params_[HASH_PARAM::SECOND_FILE]).toLocalFile();
    if (alg_id.isEmpty() || first_file.isEmpty() || second_file.isEmpty()) {
        qDebug() << "Invalid parameters";
        emit notifyWorkerFilesCompared(ERRORS::INVALID_PARAMETERS, "", "", false);
        emit notifyWorkerFinished();
        return;
    }
    HashManager hm;
    NTSTATUS status = hm.init(alg_id);
    if (status != ERROR_SUCCESS) {
        emit notifyWorkerFilesCompared(static_cast<int>(status), "", "", false);
        emit notifyWorkerFinished();
        return;
    }
    unsigned long hash_size = hm.getHashSize();
    if (hash_size == 0) {
        emit notifyWorkerFilesCompared(static_cast<int>(status), "", "", false);
        emit notifyWorkerFinished();
        return;
    }
    QByteArray first_hash;
    first_hash.resize(static_cast<int>(hash_size));
    status = calcualteHash(hm, first_file, first_hash);
    if (status != ERROR_SUCCESS) {
        emit notifyWorkerFilesCompared(static_cast<int>(status), "", "", false);
        emit notifyWorkerFinished();
        return;
    }
    QByteArray second_hash;
    second_hash.resize(static_cast<int>(hash_size));
    status = calcualteHash(hm, second_file, second_hash);
    if (status != ERROR_SUCCESS) {
        emit notifyWorkerFilesCompared(static_cast<int>(status), "", "", false);
        emit notifyWorkerFinished();
        return;
    }
    bool is_equal = first_hash == second_hash;
    emit notifyWorkerFilesCompared(ERRORS::SUCCESS, first_hash.toHex(), second_hash.toHex(), is_equal);
    emit notifyWorkerFinished();
}

long Worker::calcualteHash(HashManager& hm, const QString &file_path, QByteArray &hash)
{
    QFile file(file_path);
    if (!file.open(QFile::ReadOnly)) {
        return ERRORS::FAILED_TO_OPEN_FILE;
    }
    QByteArray data = file.readAll();
    file.close();
    NTSTATUS status = hm.createHash();
    if (status != ERROR_SUCCESS) {
        return status;
    }
    do {
        status = hm.stepHash(reinterpret_cast<uchar*>(data.data()), static_cast<ulong>(data.size()));
        if (status != ERROR_SUCCESS) {
            break;
        }
        status = hm.finishHash(reinterpret_cast<uchar*>(hash.data()), static_cast<ulong>(hash.size()));
    } while (false);
    hm.destroyHash();
    return status;
}
