#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QByteArray>

class Util
{
public:
    Util();

public:
    int readFile(const QString& path, QByteArray& content);
    int writeToFile(const QString& path, const QByteArray& content);
    QByteArray readFile2(const QString &path);
};

#endif // UTIL_H
