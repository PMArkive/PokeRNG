#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "spread.h"
#include "spreadsearch.h"
#include "pokemonrng.h"

class SearchThread : public QThread
{
    Q_OBJECT
public:
    explicit SearchThread(QObject *parent = 0);
    ~SearchThread();
    void startSearch(SpreadSearch searchParams);
    void stopSearch();
    void useNarrowSearch(bool enabled);

signals:
    void setMaxProgress(int maxProgress);
    void updateProgress(int progress);
    void foundSpread(Spread spread);
    void finished();

public slots:

protected:
    void run();

private:
    bool searchLoop();
    bool narrowSearch();
    bool narrowSearchTop();
    bool narrowSearchBottom();
    QMutex mutex;
    QWaitCondition condition;
    SpreadSearch search;
    bool stop;
    bool abort;
};

#endif // SEARCHTHREAD_H
