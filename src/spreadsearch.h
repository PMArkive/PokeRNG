#ifndef SPREADSEARCH_H
#define SPREADSEARCH_H

#include <QtGlobal>
#include "spread.h"

struct SpreadSearch
{
    enum Condition { AtLeast, Equal, AtMost, HiddenPower };
    SpreadSearch() {
        for (int i = 0; i < 34; ++i) {
            encSlots[i] = 0;
        }
    }
    bool narrow;
    bool hgss;
    bool shiny;
    quint16 trainerId;
    quint16 secretId;
    int ability;
    int nature;
    int hpType;
    int hpPower;
    int ivs[6];
    Condition conditions[6];
    int minDelay;
    int maxDelay;
    int oddEvenDelay;
    int maxMainFrame;
    int maxAltFrame;
    bool useSynch;
    bool useEncSlots;
    bool encSlots[34];
    int year;
};

#endif // SPREADSEARCH_H
