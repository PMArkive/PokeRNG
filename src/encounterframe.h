#ifndef ENCOUNTERFRAME_H
#define ENCOUNTERFRAME_H

struct EncounterFrame
{
    enum SlotType { Land = 0, Water, OldRod, GoodRod, SuperRod, RockSmash };
    EncounterFrame() {
        slotType = Land;
        slot = 0;
        synchFrame = 0;
        noSynchFrame = 0;
    }
    SlotType slotType;
    int slot;
    int synchFrame;
    int noSynchFrame;
    bool operator==(const EncounterFrame& other) const {
        return ((slot == other.slot) && (slotType == other.slotType));
    }
    bool operator<(const EncounterFrame& other) const {
        if (slotType == other.slotType) {
            return (slot < other.slot);
        }
        return (slotType < other.slotType);
    }
};

#endif // ENCOUNTERFRAME_H
