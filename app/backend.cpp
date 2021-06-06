#include "backend.h"
#include "constants.h"
#include "cryptooperations.h"
#include "util.h"
#include "errors.h"
#include "cngexceptions.h"

#include <QThread>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>

Backend::Backend(QObject *parent) : QObject(parent)
{

}

void Backend::enumHashAlgorithms()
{
    enumAlgorithms("hash", [&](int err, QStringList alg_id_list){
        emit this->notifyHashAlgsEnumComplete(err, alg_id_list);
    });
}

void Backend::calculateHash(const QString &path, const bool is_dir, const QString &alg_id)
{
    auto thread = QThread::create([&](const QString &path_, const bool is_dir_, const QString &alg_id_){
        try {
            QString local_path = QUrl(path_).toLocalFile();
            Util util;
            CryptoOperations co;
            if (!is_dir_) {
                /* QML FileDialog return path that begins with "file:///" */
                QByteArray content = util.readFile2(local_path);
                QByteArray hash = co.calcualteHash(content, alg_id_);
                emit this->notifyOneHashCalculated(ERROR_SUCCESS, alg_id_, path_, hash.toHex());
                emit this->notifyOperationFinished();
            } else {
                QDirIterator it(local_path, QDir::Files, QDirIterator::Subdirectories);
                while (it.hasNext()) {
                    QString cur_path = it.next();
                    QByteArray content = util.readFile2(cur_path);
                    QByteArray hash = co.calcualteHash(content, alg_id_);
                    emit this->notifyOneHashCalculated(ERROR_SUCCESS, alg_id_, path_, hash.toHex());
                }
                emit this->notifyOperationFinished();
            }
        } catch (CngExceptions& e) {
            emit this->notifyOneHashCalculated(e.getStatus(), alg_id_, path_, QString());
            emit this->notifyOperationFinished();
        }
    },
    path, is_dir, alg_id);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Backend::compareFiles(const QString &first_file, const QString &second_file, const QString &alg_id)
{
    auto thread = QThread::create([&](const QString &first_file_, const QString &second_file_, const QString &alg_id_) {
        QString local_path_first = QUrl(first_file_).toLocalFile();
        QString local_path_second = QUrl(second_file_).toLocalFile();
        Util util;
        CryptoOperations co;
        try {
            QByteArray content_file_first = util.readFile2(local_path_first);
            QByteArray hash_first = co.calcualteHash(content_file_first, alg_id_);
            QByteArray content_file_second = util.readFile2(local_path_second);
            QByteArray hash_second = co.calcualteHash(content_file_second, alg_id_);
            bool is_equal = (hash_first == hash_second);
            emit this->notifyFilesCompared(ERROR_SUCCESS, hash_first.toHex(), hash_second.toHex(), is_equal);
            emit this->notifyOperationFinished();
        } catch (CngExceptions& e) {
            emit this->notifyFilesCompared(e.getStatus(), QString(), QString(), false);
            emit this->notifyOperationFinished();
        }
    },
    first_file, second_file, alg_id);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

void Backend::enumRngAlgorithms()
{
    enumAlgorithms("rng", [&](int err, QStringList alg_id_list){
        emit this->notifyRngAlgsEnumComplete(err, alg_id_list);
    });
}

void Backend::generateRandom(const QString &alg_id, const QString& size, const QString &out_form)
{
    auto thread = QThread::create([&](const QString &alg_id_, const QString& size_, const QString &out_form_){
        try {
            QString bytes;
            CryptoOperations().generateRandomBytes(alg_id_, size_.toInt(), out_form_, bytes);
            emit this->notifyRandomBytesGenerated(ERROR_SUCCESS, bytes);
            emit this->notifyOperationFinished();
        } catch (CngExceptions& e) {
            emit this->notifyRandomBytesGenerated(e.getStatus(), QString());
            emit this->notifyOperationFinished();
        }
    },
    alg_id, size, out_form);
    thread->start();
}

void Backend::enumCipherAlgorithms()
{
    enumAlgorithms("cipher", [&](int err, QStringList alg_id_list){
        emit this->notifyCipherAlgsEnumCompleted(err, alg_id_list);
    });
}

void Backend::encrypt(const QString &cipher_alg_id, QString pass, const QString &plain_text_file,
                      const QString& path_to_save)
{
    auto* thread = new QThread();
    connect(thread, &QThread::started, [&, cipher_alg_id, pass, plain_text_file, path_to_save]() mutable {
        Util util;
        QByteArray plain_text;
        /* QML FileDialog return path that begins with "file:///" */
        int err = util.readFile(QUrl(plain_text_file).toLocalFile(), plain_text);
        if (err != ERRORS::SUCCESS) {
            emit notifyEncrypted(err, "");
            return;
        }
        CryptoOperations cop;
        QByteArray cipher_text, salt, iv;
        QString chaining_mode = "cbc";
        err = cop.encrypt(plain_text, pass, cipher_alg_id, chaining_mode, cipher_text, salt, iv);
        if (err != ERRORS::SUCCESS) {
            pass.fill('0');
            emit notifyEncrypted(err, "");
            return;
        }
        pass.fill('0');
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
        jobj[CIPHER_PARAM::CHAINING_MODE] = chaining_mode;
        jobj[CIPHER_PARAM::IV] = QString(iv.toBase64());
        QJsonDocument cipher_doc(jobj);
        QByteArray cipher_doc_data = cipher_doc.toJson();
        QFileInfo fi(plain_text_file);
        QString full_path = QUrl(path_to_save).toLocalFile() + "/" + fi.fileName() + ".enc";
        err = util.writeToFile(full_path, cipher_doc_data);
        emit notifyEncrypted(err, QString(cipher_doc_data));
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    pass.fill('0');
}

void Backend::decrypt(QString pass, const QString &cipher_text_file, const QString &path_to_save)
{
    auto* thread = new QThread();
    connect(thread, &QThread::started, [&, pass, cipher_text_file, path_to_save] () mutable {
        Util util;
        QByteArray container;
        /* QML FileDialog return path that begins with "file:///" */
        int err = util.readFile(QUrl(cipher_text_file).toLocalFile(), container);
        if (err != ERRORS::SUCCESS) {
            emit notifyDecrypted(err, "");
            return;
        }
        QJsonParseError jerr;
        QJsonDocument jdoc = QJsonDocument::fromJson(container, &jerr);
        if (jerr.error != QJsonParseError::NoError || jdoc.isEmpty()) {
            emit notifyDecrypted(ERRORS::CIPHERTEXT_FILE_HAS_INVALID_FORMAT, "");
            return;
        }
        QJsonObject jobj = jdoc.object();
        int iteration_count = jobj[CIPHER_PARAM::ITERATION_COUNT].toInt();
        QByteArray salt = QByteArray::fromBase64(jobj[CIPHER_PARAM::SALT].toString().toUtf8());
        QString cipher_alg_id = jobj[CIPHER_PARAM::ALG_ID].toString();
        QByteArray cipher_text = QByteArray::fromBase64(jobj[CIPHER_PARAM::CIPHER_TEXT].toString().toUtf8());
        QString chaining_mode = jobj[CIPHER_PARAM::CHAINING_MODE].toString();
        QByteArray iv = QByteArray::fromBase64(jobj[CIPHER_PARAM::IV].toString().toUtf8());
        CryptoOperations cop;
        QByteArray plain_text;
        err = cop.decrypt(cipher_text, pass, cipher_alg_id, chaining_mode, iv, iteration_count, salt, plain_text);
        if (err != ERRORS::SUCCESS) {
            pass.fill('0');
            emit notifyDecrypted(err, "");
            return;
        }
        pass.fill('0');
        QFileInfo fi(cipher_text_file);
        QString full_path = QUrl(path_to_save).toLocalFile() + "/" + fi.baseName();
        err = util.writeToFile(full_path, plain_text);
        emit notifyDecrypted(err, plain_text);
        return;
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    pass.fill('0');
}

void Backend::enumAlgorithms(const QString &type, std::function<void (int, QStringList)> notify_signal)
{
    auto thread = QThread::create([type, notify_signal](){
        try {
            QStringList alg_id_list = CryptoOperations().enumAlgorithms2(type);
            notify_signal(ERROR_SUCCESS, alg_id_list);
        } catch (CngExceptions& e) {
            notify_signal(e.getStatus(), QStringList());
        }
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}
