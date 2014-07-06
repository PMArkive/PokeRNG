#ifndef ENCOUNTERFRAMES_H
#define ENCOUNTERFRAMES_H

#define NUM_SLOTS 6

#include <QtGlobal>

class EncounterFrames
{
public:
    enum SlotType { Land = 0, Water, OldRod, GoodRod, SuperRod, RockSmash };
    EncounterFrames();
    void initialize(quint32 newSeed, quint32 newFrame, bool hgssEnabled, int spreadNature);
    void findMinFrames();
    void generateAll();
    quint32 getMinFrame(bool synchEnabled);
    quint32 getEncFrame(int slot, bool synchEnabled);
    quint32 getEncFrame(SlotType slotType, int slot, bool synchEnabled);
    bool hasGenerated();

private:
    quint16 next();
    quint16 previous();
    int getIndex(SlotType slotType, int slot);
    int getSlot(int rnd, SlotType slotType);
    void generateFrame(quint32 f, quint16 nibbleRnd, quint16 waterRnd, quint16 landRnd, bool synch);
    bool fishingSuccess(int rnd, SlotType slotType);
    bool generated;
    quint32 seed;
    quint32 frame;
    bool hgss;
    int nature;
    quint32 encFramesSynch[34];
    quint32 encFramesNoSynch[34];
    quint32 minSynch;
    quint32 minNoSynch;
};

#endif // ENCOUNTERFRAMES_H
