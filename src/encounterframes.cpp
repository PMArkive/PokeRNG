#include "encounterframes.h"

EncounterFrames::EncounterFrames()
{
    generated = false;
}

void EncounterFrames::initialize(quint32 newSeed, quint32 newFrame, bool hgssEnabled, int spreadNature)
{
    //qDebug("Initialising encounter frames for seed %08X, frame %d, hgss %d, nature %d", newSeed, newFrame, hgssEnabled, spreadNature);
    for (int i = 0; i < (34); ++i) {
        encFramesSynch[i] = 0;
        encFramesNoSynch[i] = 0;
    }
    seed = newSeed;
    frame = newFrame;
    hgss = hgssEnabled;
    nature = spreadNature;
    generated = false;
}

void EncounterFrames::findMinFrames()
{
    int frameNature;
    quint16 rnd;
    quint16 rnd2;
    quint32 pid;
    bool synchActive;
    bool synchActive2;
    previous();
    previous();
    previous();
    --frame;
    minSynch = 0;
    minNoSynch = 0;
    generated = true;
    while (frame > 0) {
        rnd = previous();
        rnd2 = previous();
        if (hgss) {
            frameNature = rnd % 25;
            synchActive = ((rnd & 1) == 0);
            synchActive2 = ((rnd2 & 1) == 0);
        } else {
            frameNature = rnd / 0xA3E;
            synchActive = ((rnd >> 15) == 0);
            synchActive2 = ((rnd2 >> 15) == 0);
        }
        if ((frame > 2) && !synchActive2 && (frameNature == nature)) {
            minSynch = frame - 1;
        } else if (synchActive) {
            minSynch = frame;
        }
        if (frameNature == nature) {
            minNoSynch = frame;
        }
        pid = rnd2 | (rnd << 16);
        if ((pid % 25) == nature) {
            return;
        }
        frame -= 2;
    }
}

void EncounterFrames::generateAll()
{
    int frameNature;
    quint16 prev1;
    quint16 prev2;
    quint16 prev3;
    quint16 prev4;
    quint16 prev5;
    quint32 pid;
    bool synchActive;
    bool synchActive2;
    previous();
    previous();
    previous();
    prev3 = previous();
    prev2 = previous();
    prev1 = previous();
    --frame;
    minSynch = 0;
    minNoSynch = 0;
    generated = true;
    while (frame > 0) {
        prev5 = prev3;
        prev4 = prev2;
        prev3 = prev1;
        prev2 = previous();
        prev1 = previous();
        if (hgss) {
            frameNature = prev5 % 25;
            synchActive = ((prev5 & 1) == 0);
            synchActive2 = ((prev4 & 1) == 0);
        } else {
            frameNature = prev5 / 0xA3E;
            synchActive = ((prev5 >> 15) == 0);
            synchActive2 = ((prev4 >> 15) == 0);
        }
        if ((frame > 2) && !synchActive2 && (frameNature == nature)) {
            minSynch = frame - 1;
            generateFrame(frame - 1, prev1, prev2, prev3, true);
        } else if (synchActive) {
            minSynch = frame;
            generateFrame(frame, prev2, prev3, prev4, true);
        }
        if (frameNature == nature) {
            minNoSynch = frame;
            generateFrame(frame, prev2, prev3, prev4, false);
        }
        pid = prev4 | (prev5 << 16);
        if ((frame == 1) || ((pid % 25) == nature)) {
            return;
        }
        frame -= 2;
    }
}

quint32 EncounterFrames::getMinFrame(bool synchEnabled)
{
    return synchEnabled ? minSynch : minNoSynch;
}

quint32 EncounterFrames::getEncFrame(int slot, bool synchEnabled)
{
    return synchEnabled ? encFramesSynch[slot] : encFramesNoSynch[slot];
}

quint32 EncounterFrames::getEncFrame(SlotType slotType, int slot, bool synchEnabled)
{
    return synchEnabled ? encFramesSynch[getIndex(slotType, slot)] : encFramesNoSynch[getIndex(slotType, slot)];
}

bool EncounterFrames::hasGenerated()
{
    return generated;
}

quint16 EncounterFrames::next()
{
    seed = seed * 0x41C64E6D + 0x6073;
    return (seed >> 16);
}

quint16 EncounterFrames::previous()
{
    seed = seed  * 0xEEB9EB65 + 0xA3561A1;
    return (seed >> 16);
}

int EncounterFrames::getIndex(SlotType slotType, int slot)
{
    int index = slotType * 5;
    if (index > 0) {
        index += 7;
    }
    index += slot;
    return index;
}

int EncounterFrames::getSlot(int rnd, SlotType slotType)
{
    if (hgss) {
        rnd = rnd % 100;
    } else {
        rnd = rnd / 0x290;
    }
    switch (slotType) {
    case Land:
        if (rnd < 20) {
            return 0;
        } else if (rnd < 40) {
            return 1;
        } else if (rnd < 50) {
            return 2;
        } else if (rnd < 60) {
            return 3;
        } else if (rnd < 70) {
            return 4;
        } else if (rnd < 80) {
            return 5;
        } else if (rnd < 85) {
            return 6;
        } else if (rnd < 90) {
            return 7;
        } else if (rnd < 94) {
            return 8;
        } else if (rnd < 98) {
            return 9;
        } else if (rnd < 99) {
            return 10;
        } else {
            return 11;
        }
    case Water:
        if (rnd < 60) {
            return 0;
        } else if (rnd < 90) {
            return 1;
        } else if (rnd < 95) {
            return 2;
        } else if (rnd < 99) {
            return 3;
        } else {
            return 4;
        }
    case OldRod:
        if (hgss) {
            if (rnd < 40) {
                return 0;
            } else if (rnd < 70) {
                return 1;
            } else if (rnd < 85) {
                return 2;
            } else if (rnd < 95) {
                return 3;
            } else {
                return 4;
            }
        } else {
            if (rnd < 60) {
                return 0;
            } else if (rnd < 90) {
                return 1;
            } else if (rnd < 95) {
                return 2;
            } else if (rnd < 99) {
                return 3;
            } else {
                return 4;
            }
        }
    case GoodRod:
    case SuperRod:
        if (hgss) {
            if (rnd < 40) {
                return 0;
            } else if (rnd < 70) {
                return 1;
            } else if (rnd < 85) {
                return 2;
            } else if (rnd < 95) {
                return 3;
            } else {
                return 4;
            }
        } else {
            if (rnd < 40) {
                return 0;
            } else if (rnd < 40) {
                return 1;
            } else if (rnd < 95) {
                return 2;
            } else if (rnd < 99) {
                return 3;
            } else {
                return 4;
            }
        }
    case RockSmash:
        if (rnd < 90) {
            return 0;
        } else {
            return 1;
        }
    }
    return 0;
}

void EncounterFrames::generateFrame(quint32 f, quint16 nibbleRnd, quint16 waterRnd, quint16 landRnd, bool synch)
{
    int slot;
    if (f > 1) {
        slot = getSlot(landRnd, Land);
        if (synch) {
            encFramesSynch[getIndex(Land, slot)] = f;
        } else {
            encFramesNoSynch[getIndex(Land, slot)] = f;
        }
    }
    if (f > 2) {
        slot = getSlot(waterRnd, Water);
        if (synch) {
            encFramesSynch[getIndex(Water, slot)] = f;
        } else {
            encFramesNoSynch[getIndex(Water, slot)] = f;
        }
    }
    if (f > 3) {
        if (fishingSuccess(nibbleRnd, SuperRod)) {
            slot = getSlot(waterRnd, SuperRod);
            if (synch) {
                encFramesSynch[getIndex(SuperRod, slot)] = f;
            } else {
                encFramesNoSynch[getIndex(SuperRod, slot)] = f;
            }
        }
        if (fishingSuccess(nibbleRnd, GoodRod)) {
            slot = getSlot(waterRnd, GoodRod);
            if (synch) {
                encFramesSynch[getIndex(GoodRod, slot)] = f;
            } else {
                encFramesNoSynch[getIndex(GoodRod, slot)] = f;
            }
        }
        if (fishingSuccess(nibbleRnd, OldRod)) {
            slot = getSlot(waterRnd, OldRod);
            if (synch) {
                encFramesSynch[getIndex(OldRod, slot)] = f;
            } else {
                encFramesNoSynch[getIndex(OldRod, slot)] = f;
            }
        }
        if (hgss && ((nibbleRnd % 100) < 20)) { // Tentative
            slot = getSlot(waterRnd, RockSmash);
            if (synch) {
                encFramesSynch[getIndex(RockSmash, slot)] = f;
            } else {
                encFramesNoSynch[getIndex(RockSmash, slot)] = f;
            }
        }
    }
}

bool EncounterFrames::fishingSuccess(int rnd, SlotType slotType)
{
    if (hgss) {
        rnd = rnd % 100; // Tentative
    } else {
        rnd = rnd / 0x290;
    }
    switch (slotType) {
    case SuperRod:
        return (rnd < 75);
    case GoodRod:
        return (rnd < 50);
    case OldRod:
        return (rnd < 25);
    default:
        return false;
    }
}
