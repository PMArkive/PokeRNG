#include "mersennetwister.h"

MersenneTwister::MersenneTwister(quint32 seed)
{
    seeds[0] = seed;
    quint64 temp;
    for (int i = 1; i < 624; ++i) {
        temp = ((seeds[i - 1] >> 30) ^ seeds[i - 1]) * 0x6C078965 + i;
        seeds[i] = temp;
    }
    frame = 0;
}

quint32 MersenneTwister::getNext()
{
    if (frame == 0) {
        twist();
    }
    quint32 ret;
    ret = (seeds[frame] >> 11) ^ seeds[frame];
    ret = ((ret << 7) & 0x9D2C5680) ^ ret;
    ret = ((ret << 15) & 0xEFC60000) ^ ret;
    ret = (ret >> 18) ^ ret;
    ++frame;
    if (frame >= 624) {
        frame = 0;
    }
    return ret;
}

void MersenneTwister::twist()
{
    quint32 a;
    quint32 b;
    for (int i = 0; i < 624; ++i) {
        a = (seeds[i] & 0x80000000) | (seeds[(i + 1) % 624] & 0x7FFFFFFF);
        b = (a >> 1) ^ seeds[(i + 397) % 624];
        if ((a & 1) == 0) {
            seeds[i] = b;
        } else {
            seeds[i] = b ^ 0x9908B0DF;
        }
    }
}
