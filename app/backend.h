#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

signals:
    void notifyHashAlsEnumComplete(int err, QStringList alg_id_list);

public slots:
    void enumHashAlgorithms();
    void calculateHash(const QString& path, const bool is_dir, const QString& alg_id);

};

#endif // BACKEND_H
