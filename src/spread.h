#ifndef SPREAD_H
#define SPREAD_H

#include <QtGlobal>
#include <QStringList>

struct Spread
{
    static const QStringList NatureNames;
    static const QStringList TypeNames;
    const int nature() const {
        return (pid % 25);
    }
    const int ability() const {
        return (pid & 1);
    }
    const int shiny() const {
        quint16 val;
        val = pid & 0xFFFF;
        val = val ^ (pid >> 16);
        val = val ^ trainerId;
        val = val ^ secretId;
        return (val < 8);
    }
    const int hpType() const {
        int type = (ivs[0] & 1);
        type |= ((ivs[1] & 1) << 1);
        type |= ((ivs[2] & 1) << 2);
        type |= ((ivs[3] & 1) << 3);
        type |= ((ivs[4] & 1) << 4);
        type |= ((ivs[5] & 1) << 5);
        return ((type * 15) / 63);
    }
    const int hpPower() const {
        int power = (ivs[0] & 2) >> 1;
        power |= (ivs[1] & 2);
        power |= ((ivs[2] & 2) << 1);
        power |= ((ivs[3] & 2) << 2);
        power |= ((ivs[4] & 2) << 3);
        power |= ((ivs[5] & 2) << 4);
        return (((power * 40) / 63) + 30);
    }
    const int delay() const {
        return (seed & 0xFFFF);
    }
    quint16 trainerId;
    quint16 secretId;
    quint32 pid;
    int ivs[6];
    quint32 seed;
    quint32 mSeed;
    int mainFrame;
    int altFrameSynch;
    int altFrameNoSynch;
};

#endif // SPREAD_H
