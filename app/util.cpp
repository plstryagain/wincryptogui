#include "util.h"
#include "errors.h"

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
