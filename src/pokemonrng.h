#ifndef POKEMONRNG_H
#define POKEMONRNG_H

#include <QtGlobal>

class Spread;

class PokemonRNG
{
public:
    PokemonRNG();
    bool update();
    void setDelayLimits(int min, int max);
    bool isValidSeed();
    bool started();
    int getFrame();
    void generate(Spread& spread);
    void finalize(Spread& spread);
    void initialize();
    quint32 getSeed();
    quint16 getRand();
    quint16 getPrevious();
    void setHgss(bool enabled);
    int getAltFrameSynch(int nature);
    int getAltFrameNoSynch(int nature);
    void setSeed(quint32 value);
    void setFrame(int value);
private:
    void callRng();
    void getIvs(int* ivs);
    quint32 getPid();
    bool hgss;
    int frame;
    int seedCounter;
    int minDelay;
    int maxDelay;
    quint32 seed;
    quint32 lastValidSeed;
    quint32 firstSeed;
    quint32 newSeed;
    quint16 results[4];
};

#endif // POKEMONRNG_H
