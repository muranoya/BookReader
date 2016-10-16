#ifndef PREFETCHER_HPP
#define PREFETCHER_HPP

#include <QThread>
#include <QVector>
#include <QByteArray>
#include <QCache>
#include <QMutex>
#include <QWaitCondition>
#include "ImageFile.hpp"

class Prefetcher : public QThread
{
public:
    explicit Prefetcher(QObject *parent = 0);
    ~Prefetcher();

    void putRequest(const QVector<ImageFile> &args);
    QByteArray *get(const QString &key);
    void setCacheSize(int n);
    int getCacheSize() const;

protected:
    void run();

private:
    class Worker : public QThread
    {
    public:
        explicit Worker(Prefetcher *parent);
        ~Worker();
    protected:
        void run();
    private:
        Prefetcher *master;
    };
    
    Worker *worker[8];
    QCache<QString, QByteArray> cache;
    QVector<ImageFile> files;
    QVector<ImageFile> reqfiles;

    QMutex mutex;
    QMutex mutex_req;
    QWaitCondition cond_get;
    QWaitCondition cond_put;
    QWaitCondition cond_req;
    int task_no;
    int task_filled;

    ImageFile *getTask();
    void setResult(const QString &key, QByteArray *data);
};

#endif // PREFETCHER_HPP
