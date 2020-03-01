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
};

#endif // UTIL_H
