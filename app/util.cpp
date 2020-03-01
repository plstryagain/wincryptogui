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
