#ifndef MERSENNETWISTER_H
#define MERSENNETWISTER_H

#include <QtGlobal>

class MersenneTwister
{
public:
    MersenneTwister(quint32 seed);
    quint32 getNext();

private:
    void twist();
    quint32 seeds[624];
    int frame;
};

#endif // MERSENNETWISTER_H
