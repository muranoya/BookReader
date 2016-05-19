#ifndef PREFETCHER_H
#define PREFETCHER_H

#include <QThread>
#include <QMutex>
#include <QCache>
#include <QByteArray>
#include <QStringList>

class Prefetcher : public QThread
{
Q_OBJECT
public:
    Prefetcher(QCache<QString, QByteArray> *ch, QMutex *m);
    ~Prefetcher();

    void setPrefetchImage(QStringList &list);
    void sendTermSig();

signals:
    void prefetchFinished();

protected:
    virtual void run();

private:
    QCache<QString, QByteArray> *cache;
    QMutex *mutex;
    QStringList plist;
    bool termsig;
};

#endif
