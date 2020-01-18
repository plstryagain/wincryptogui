#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

signals:
    void notifyHashAlsEnumComplete(long err, QStringList alg_id_list);

public slots:
    void enumHashAlgorithms();

};

#endif // BACKEND_H
