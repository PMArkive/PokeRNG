#include "pokemonrng.h"
#include "spread.h"

PokemonRNG::PokemonRNG()
{
}

bool PokemonRNG::update()
{
    ++frame;
    callRng();
    if (seedCounter > 0) {
        --seedCounter;
        if (seedCounter == 0) {
            if (firstSeed == 0) {
                firstSeed = newSeed;
            }
            lastValidSeed = newSeed;
            frame = 1;
        }
    }
    if (isValidSeed()) {
        seedCounter = 4;
        newSeed = seed;
    }
    return !(seed == firstSeed);
}

void PokemonRNG::setDelayLimits(int min, int max)
{
    minDelay = min;
    maxDelay = max;
}

bool PokemonRNG::isValidSeed()
{
    int seedDelay = (seed & 0xFFFF);
    return ((((seed >> 16) & 0xFF) < 24) && (seedDelay <= maxDelay) && (seedDelay >= minDelay));
}

bool PokemonRNG::started()
{
    return (firstSeed > 0);
}

int PokemonRNG::getFrame()
{
    return frame;
}

void PokemonRNG::generate(Spread& spread)
{
    getIvs(spread.ivs);
    spread.pid = getPid();
    spread.mainFrame = frame;
    spread.seed = lastValidSeed;
}

void PokemonRNG::finalize(Spread& spread)
{
    spread.altFrameSynch = getAltFrameSynch(spread.nature());
    spread.altFrameNoSynch = getAltFrameNoSynch(spread.nature());
}

void PokemonRNG::initialize()
{
    seed = 0;
    seedCounter = 0;
    firstSeed = 0;
    lastValidSeed = 0;
    frame = 0;
    callRng();
    callRng();
    callRng();
    callRng();
}

quint32 PokemonRNG::getSeed()
{
    return seed;
}

quint16 PokemonRNG::getRand() {
    seed = seed * 0x41C64E6D + 0x6073;
    return (seed >> 16);
}

quint16 PokemonRNG::getPrevious() {
    seed = seed * 0xEEB9EB65 + 0xA3561A1;
    return (seed >> 16);
}

int PokemonRNG::getAltFrameNoSynch(int nature)
{
    quint32 oldSeed = seed;
    int altFrame = frame;
    int bestFrame = 0;
    int frameNature;
    quint16 rnd;
    quint16 rnd2;
    quint32 pid;
    getPrevious();
    getPrevious();
    getPrevious();
    --altFrame;
    while (altFrame > 1) {
        rnd = getPrevious();
        rnd2 = getPrevious();
        if (hgss) {
            frameNature = rnd % 25;
        } else {
            frameNature = rnd / 0xA3E;
        }
        if (frameNature == nature) {
            bestFrame = altFrame;
        }
        pid = rnd2 | (rnd << 16);
        if ((pid % 25) == nature) {
            seed = oldSeed;
            return bestFrame;
        }
        altFrame -= 2;
    }
    seed = oldSeed;
    return bestFrame;
}

int PokemonRNG::getAltFrameSynch(int nature)
{
    quint32 oldSeed = seed;
    int altFrame = frame;
    int bestFrame = 0;
    int frameNature;
    quint16 rnd;
    quint16 rnd2;
    quint32 pid;
    bool synchActive;
    bool synchActive2;
    getPrevious();
    getPrevious();
    getPrevious();
    altFrame -= 1;
    while (altFrame > 1) {
        rnd = getPrevious();
        rnd2 = getPrevious();
        if (hgss) {
            frameNature = rnd % 25;
            synchActive = ((rnd & 1) == 0);
            synchActive2 = ((rnd2 & 1) == 0);
        } else {
            frameNature = rnd / 0xA3E;
            synchActive = ((rnd >> 15) == 0);
            synchActive2 = ((rnd2 >> 15) == 0);
        }
        if ((altFrame > 3) && !synchActive2 && (frameNature == nature)) {
            bestFrame = altFrame - 1;
        } else if (synchActive) {
            bestFrame = altFrame;
        }
        pid = rnd2 | (rnd << 16);
        if ((pid % 25) == nature) {
            seed = oldSeed;
            return bestFrame;
        }
        altFrame -= 2;
    }
    seed = oldSeed;
    return bestFrame;
}

void PokemonRNG::setHgss(bool enabled)
{
    hgss = enabled;
}

void PokemonRNG::setSeed(quint32 value)
{
    seed = value;
}

void PokemonRNG::setFrame(int value)
{
    frame = value;
}

void PokemonRNG::callRng()
{
    results[3] = results[2];
    results[2] = results[1];
    results[1] = results[0];
    results[0] = getRand();
}

void PokemonRNG::getIvs(int *ivs)
{
    ivs[0] = (results[1] & 0x1F); // The 6 IVs are generated using two consecutive RNG calls; 5 bits are used per stat.
    ivs[1] = ((results[1] >> 5) & 0x1F);
    ivs[2] = ((results[1] >> 10) & 0x1F);
    ivs[3] = (results[0] & 0x1F);
    ivs[4] = ((results[0] >> 5) & 0x1F);
    ivs[5] = ((results[0] >> 10) & 0x1F);
}

quint32 PokemonRNG::getPid()
{
    //quint32 pid;
    //pid = results[3] | (results[2] << 0x10);
    return results[3] | (results[2] << 0x10);
}
