#include "util.h"
#include "errors.h"
#include "cngexceptions.h"

#include <QFile>

Util::Util()
{

}

int Util::readFile(const QString &path, QByteArray &content)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        return ERRORS::FAILED_TO_OPEN_FILE;
    }
    content = file.readAll();
    file.close();
    if (content.isEmpty()) {
        return ERRORS::FAILED_TO_READ_FILE;
    }
    return ERRORS::SUCCESS;
}

QByteArray Util::readFile2(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        throw CngExceptions(ERROR_OPEN_FAILED, "Failed to open file");
    }
    QByteArray content = file.readAll();
    file.close();
    if (content.isEmpty()) {
        throw CngExceptions(ERROR_READ_FAULT, "Failed to read file");
    }
    return content;
}

int Util::writeToFile(const QString &path, const QByteArray &content)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        return ERRORS::FAILED_TO_OPEN_FILE;
    }
    if (file.write(content) == -1) {
        file.close();
        return ERRORS::FAILED_WRITE_TO_FILE;
    }
    file.close();
    return  ERRORS::SUCCESS;
}
