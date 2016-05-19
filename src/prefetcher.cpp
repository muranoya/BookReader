#include "prefetcher.hpp"
#include <QFile>

Prefetcher::Prefetcher(QCache<QString, QByteArray> *ch, QMutex *m)
    : QThread()
    , cache(ch)
    , mutex(m)
    , termsig(false)
{
}

Prefetcher::~Prefetcher()
{
}

void
Prefetcher::setPrefetchImage(QStringList &list)
{
    plist = QStringList(list);
}

void
Prefetcher::sendTermSig()
{
    termsig = true;
}

void
Prefetcher::run()
{
    termsig = false;
    for (QStringList::const_iterator i = plist.constBegin();
            i != plist.constEnd(); ++i)
    {
        if (termsig) return;

        QString path = *i;
        mutex->lock();
        bool c = cache->contains(path);
        mutex->unlock();

        if (!c)
        {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly))
            {
                QByteArray *data = new QByteArray(file.readAll());
                mutex->lock();
                if (cache->maxCost() > 0)
                {
                    cache->insert(path, data, 1);
                }
                else
                {
                    mutex->unlock();
                    emit prefetchFinished();
                    return;
                }
                mutex->unlock();
            }
        }
    }
    emit prefetchFinished();
}

