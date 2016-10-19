#include "Prefetcher.hpp"

Prefetcher::Prefetcher(QObject *parent)
    : QThread(parent)
    , cache(20)
    , task_no(0)
    , task_filled(0)
{
    start();

    int len = sizeof(worker)/sizeof(worker[0]);
    for (int i = 0; i < len; ++i)
    {
        worker[i] = new Worker(this);
        worker[i]->start();
    }
}

Prefetcher::~Prefetcher()
{
    int len = sizeof(worker)/sizeof(worker[0]);
    for (int i = 0; i < len; ++i)
    {
        delete worker[i];
    }
    cache.clear();
    files.clear();
    reqfiles.clear();
}

void
Prefetcher::putRequest(const QVector<ImageFile> &tasks)
{
    mutex_req.lock();
    reqfiles.clear();
    reqfiles = tasks;
    cond_req.wakeOne();
    mutex_req.unlock();
}

QByteArray *
Prefetcher::get(const QString &key)
{
    QByteArray *data = nullptr;
    mutex.lock();
    if (cache.contains(key)) data = cache[key];
    mutex.unlock();
    return data;
}

void
Prefetcher::setCacheSize(int n)
{
    mutex.lock();
    cache.setMaxCost(n);
    mutex.unlock();
}

int
Prefetcher::getCacheSize() const
{
    return cache.maxCost();
}

void
Prefetcher::run()
{
    for (;;)
    {
        mutex_req.lock();
        for (;;)
        {
            while (reqfiles.empty() || getCacheSize() == 0)
            {
                cond_req.wait(&mutex_req);
            }

            mutex.lock();
            if (task_filled >= files.count())
            {
                task_no = 0;
                task_filled = 0;
                files.clear();
                files = reqfiles;
                reqfiles.clear();
                cond_get.wakeAll();
                mutex.unlock();
                break;
            }
            mutex.unlock();
        }
        mutex_req.unlock();

        mutex.lock();
        while (task_filled < files.count())
        {
            cond_put.wait(&mutex);
        }
        mutex.unlock();
    }
}

ImageFile *
Prefetcher::getTask()
{
    ImageFile *f;
    mutex.lock();
    for (;;)
    {
        while (task_no >= files.count())
        {
            cond_get.wait(&mutex);
        }
        f = &files[task_no];
        task_no++;

        QString k = f->createKey();
        if (cache.contains(k))
        {
            cache[k]; // update
            task_filled++;
            if (task_filled >= files.count())
            {
                cond_put.wakeOne();
            }
        }
        else
        {
            break;
        }
    }
    mutex.unlock();
    return f;
}

void
Prefetcher::setResult(const QString &key, QByteArray *data)
{
    mutex.lock();
    cache.insert(key, data, 1);
    task_filled++;
    if (task_filled >= files.count())
    {
        cond_put.wakeOne();
    }
    mutex.unlock();
}

Prefetcher::Worker::Worker(Prefetcher *parent)
    : master(parent)
{ }

Prefetcher::Worker::~Worker() { }

void
Prefetcher::Worker::run()
{
    for (;;)
    {
        ImageFile *f = master->getTask();
        QByteArray *data = f->readData();
        master->setResult(f->createKey(), data);
    }
}

