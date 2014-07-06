#include "searchthread.h"
#include "encounterframes.h"
#include <QMutexLocker>

SearchThread::SearchThread(QObject *parent) :
    QThread(parent)
{
    stop = false;
    abort = false;
}

SearchThread::~SearchThread()
{
    mutex.lock();
    stop = true;
    abort = true;
    condition.wakeAll();
    mutex.unlock();
    wait();
}

void SearchThread::startSearch(SpreadSearch searchParams)
{
    QMutexLocker locker(&mutex);
    search = searchParams;
    stop = false;
    if (!isRunning()) {
        start(LowPriority);
    } else {
        condition.wakeAll();
    }
}

void SearchThread::stopSearch()
{
    stop = true;
}

void SearchThread::run()
{
    forever {
        if (stop) {
            if (abort) {
                return;
            }
            condition.wait(&mutex);
        }
        if (search.narrow) {
            if (narrowSearch()) {
                emit finished();
            }
        } else {
            if (searchLoop()) {
                emit finished();
            }
        }
        stop = true;
    }
}

bool SearchThread::searchLoop()
{
    int srcHp = search.ivs[0];
    int srcAtk = search.ivs[1];
    int srcDef = search.ivs[2];
    int srcSpe = search.ivs[3];
    int srcSpA = search.ivs[4];
    int srcSpD = search.ivs[5];
    int hpHp = srcHp & 3;
    int hpAtk = srcAtk & 3;
    int hpDef = srcDef & 3;
    int hpSpe = srcSpe & 3;
    int hpSpA = srcSpA & 3;
    int hpSpD = srcSpD & 3;
    int cndHp = search.conditions[0];
    int cndAtk = search.conditions[1];
    int cndDef = search.conditions[2];
    int cndSpe = search.conditions[3];
    int cndSpA = search.conditions[4];
    int cndSpD = search.conditions[5];
    bool hgss = search.hgss;
    bool shiny = search.shiny;
    quint16 tid = search.trainerId;
    quint16 sid = search.secretId;
    int srcNature = search.nature;
    int srcAbility = search.ability;
    int srcHpType = search.hpType;
    int srcHpPower = search.hpPower;
    int year = search.year;
    quint16 minDelay = search.minDelay + year;
    quint16 maxDelay = search.maxDelay + year;
    int oddEvenDelay = search.oddEvenDelay;
    minDelay = (minDelay & 0xFFF7) | (search.oddEvenDelay & 1);
    maxDelay = (maxDelay & 0xFFF7) | (search.oddEvenDelay & 1);
    quint32 maxMainFrame = search.maxMainFrame;
    quint32 maxAltFrame = search.maxAltFrame;
    bool useSynch = search.useSynch;
    bool useEncSlots = search.useEncSlots;
    bool encSlots[34];
    for (int i = 0; i < 34; ++i) {
        encSlots[i] = search.encSlots[i];
    }
    int numEncSlots = hgss ? (34) : (32);
    quint16 rnd1;
    quint16 rnd2;
    quint16 rnd3;
    quint16 rnd4;
    int rndHp;
    int rndAtk;
    int rndDef;
    int rndSpA;
    int rndSpD;
    int rndSpe;
    int rndHpPower;
    int rndHpType;
    quint32 newSeed;
    quint32 lastValidSeed;
    quint32 seed;
    int seedCounter;
    quint32 frame;
    quint32 pid;
    quint32 altFrame;
    quint32 altFrameTemp;
    bool foundSlot;
    int progress;
    Spread spread;
    EncounterFrames encFrames;
    spread.trainerId = tid;
    spread.secretId = sid;
    lastValidSeed = minDelay;
    seed = lastValidSeed;
    rnd4 = seed >> 16;
    seed *= 0x41C64E6D;
    seed += 0x6073;
    rnd3 = seed >> 16;
    seed *= 0x41C64E6D;
    seed += 0x6073;
    rnd2 = seed >> 16;
    seed *= 0x41C64E6D;
    seed += 0x6073;
    rnd1 = seed >> 16;
    seedCounter = 0;
    frame = 0;
    progress = 0;
    emit setMaxProgress(1024);
    while (true) {
        if (stop || abort) {
            return false;
        }
        ++frame;
        seed *= 0x41C64E6D;
        seed += 0x6073;
        rnd4 = rnd3;
        rnd3 = rnd2;
        rnd2 = rnd1;
        rnd1 = seed >> 16;
        if (seedCounter > 0) {
            --seedCounter;
            if (seedCounter == 0) {
                if (newSeed == minDelay) {
                    return true;
                }
                lastValidSeed = newSeed;
                frame = 1;
            }
        }
        if ((((seed >> 16) & 0xFF) < 24) && ((seed & 0xFFFF) >= minDelay) && ((seed & 0xFFFF) <= maxDelay) && ((oddEvenDelay == 0) || ((((seed & 0xFFFF) - year) & 1) == (oddEvenDelay & 1)))) {
            seedCounter = 4;
            newSeed = seed;
        }
        if ((seed & 0x3FFFFF) == 0) {
            ++progress;
            emit updateProgress(progress);
        }
        if ((maxMainFrame > 0) && (frame > maxMainFrame)) {
            continue;
        }
        if (shiny && ((rnd4 ^ rnd3 ^ tid ^ sid) >= 8)) {
            continue;
        }
        rndHp = (rnd2 & 0x1F);
        rndAtk = ((rnd2 >> 5) & 0x1F);
        rndDef = ((rnd2 >> 10) & 0x1F);
        rndSpe = (rnd1 & 0x1F);
        rndSpA = ((rnd1 >> 5) & 0x1F);
        rndSpD = ((rnd1 >> 10) & 0x1F);
        if ((((cndHp == 0) && (rndHp >= srcHp)) || ((cndHp == 1) && (rndHp == srcHp)) || ((cndHp == 2) && (rndHp <= srcHp)) || ((cndHp == 3) && (rndHp >= srcHp) && ((rndHp & 3) == hpHp))) &&
                (((cndAtk == 0) && (rndAtk >= srcAtk)) || ((cndAtk == 1) && (rndAtk == srcAtk)) || ((cndAtk == 2) && (rndAtk <= srcAtk)) || ((cndAtk == 3) && (rndAtk >= srcAtk) && ((rndAtk & 3) == hpAtk))) &&
                (((cndDef == 0) && (rndDef >= srcDef)) || ((cndDef == 1) && (rndDef == srcDef)) || ((cndDef == 2) && (rndDef <= srcDef)) || ((cndDef == 3) && (rndDef >= srcDef) && ((rndDef & 3) == hpDef))) &&
                (((cndSpe == 0) && (rndSpe >= srcSpe)) || ((cndSpe == 1) && (rndSpe == srcSpe)) || ((cndSpe == 2) && (rndSpe <= srcSpe)) || ((cndSpe == 3) && (rndSpe >= srcSpe) && ((rndSpe & 3) == hpSpe))) &&
                (((cndSpA == 0) && (rndSpA >= srcSpA)) || ((cndSpA == 1) && (rndSpA == srcSpA)) || ((cndSpA == 2) && (rndSpA <= srcSpA)) || ((cndSpA == 3) && (rndSpA >= srcSpA) && ((rndSpA & 3) == hpSpA))) &&
                (((cndSpD == 0) && (rndSpD >= srcSpD)) || ((cndSpD == 1) && (rndSpD == srcSpD)) || ((cndSpD == 2) && (rndSpD <= srcSpD)) || ((cndSpD == 3) && (rndSpD >= srcSpD) && ((rndSpD & 3) == hpSpD)))) {
            if (srcNature >= 0) {
                pid = rnd4 | (rnd3 << 16);
                if ((pid % 25) != srcNature) {
                    continue;
                }
            }
            if (srcAbility >= 0) {
                if ((rnd4 & 1) != srcAbility) {
                    continue;
                }
            }
            if (srcHpPower > 30) {
                rndHpPower = (((rndHp & 2) >> 1) | (rndAtk & 2) | ((rndDef & 2) << 1) | ((rndSpe & 2) << 2) | ((rndSpA & 2) << 3) | ((rndSpD & 2) << 4));
                rndHpPower = (((rndHpPower * 40) / 63) + 30);
                if (rndHpPower < srcHpPower) {
                    continue;
                }
            }
            if (srcHpType >= 0) {
                rndHpType = ((rndHp & 1) | ((rndAtk & 1) << 1) | ((rndDef & 1) << 2) | ((rndSpe & 1) << 3) | ((rndSpA & 1) << 4) | ((rndSpD & 1) << 5));
                rndHpType = ((rndHpType * 15) / 63);
                if (rndHpType != srcHpType) {
                    continue;
                }
            }
            pid = rnd4 | (rnd3 << 16);
            spread.pid = pid;
            encFrames.initialize(seed, frame, search.hgss, spread.nature());
            if (useEncSlots) {
                encFrames.generateAll();
            } else {
                encFrames.findMinFrames();
            }
            if (maxAltFrame > 0) {
                if (useSynch) {
                    altFrame = encFrames.getMinFrame(true);
                    altFrameTemp = encFrames.getMinFrame(false);
                    if ((altFrameTemp > 0) && (altFrameTemp < altFrame)) {
                        altFrame = altFrameTemp;
                    }
                } else {
                    altFrame = encFrames.getMinFrame(false);
                }
                if ((altFrame == 0) || (altFrame > maxAltFrame)) {
                    continue;
                }
            }
            if (useEncSlots) {
                altFrame = 0;
                foundSlot = false;
                for (int i = 0; i < numEncSlots; ++i) {
                    if (!encSlots[i]) {
                        continue;
                    }
                    if (useSynch) {
                        altFrame = encFrames.getEncFrame(i, true);
                    }
                    altFrameTemp = encFrames.getEncFrame(i, false);
                    if ((altFrameTemp > 0) || (altFrame > 0)) {
                        foundSlot = true;
                        break;
                    }
                }
                if (!foundSlot) {
                    continue;
                }
            }
            spread.ivs[0] = rndHp;
            spread.ivs[1] = rndAtk;
            spread.ivs[2] = rndDef;
            spread.ivs[3] = rndSpe;
            spread.ivs[4] = rndSpA;
            spread.ivs[5] = rndSpD;
            spread.mainFrame = frame;
            spread.mSeed = seed;
            spread.seed = lastValidSeed;
            spread.altFrameSynch = encFrames.getMinFrame(true);
            spread.altFrameNoSynch = encFrames.getMinFrame(false);
            emit foundSpread(spread);
        }
    }
}

bool SearchThread::narrowSearch()
{
    int ranges[6];
    for (int i = 0; i < 6; ++i) {
        switch (search.conditions[i]) {
        case 0:
            ranges[i] = 32 - search.ivs[i];
            break;
        case 1:
            ranges[i] = 1;
            break;
        case 2:
            ranges[i] = search.ivs[i] + 1;
            break;
        case 3:
            ranges[i] = search.ivs[i] / 4 + 1;
            break;
        }
    }
    int range1 = ranges[0] * ranges[1] * ranges[2];
    int range2 = ranges[3] * ranges[4] * ranges[5];
    if (range1 <= range2) {
        emit setMaxProgress(ranges[0] * ranges[1]);
        return narrowSearchTop();
    } else {
        emit setMaxProgress(ranges[3] * ranges[4]);
        return narrowSearchBottom();
    }
}

bool SearchThread::narrowSearchTop()
{
    int min[3];
    int max[3];
    for (int i = 0; i < 3; ++i) {
        switch (search.conditions[i]) {
        case 0:
            min[i] = search.ivs[i];
            max[i] = 31;
            break;
        case 1:
            min[i] = search.ivs[i];
            max[i] = search.ivs[i];
            break;
        case 2:
            min[i] = 0;
            max[i] = search.ivs[i];
            break;
        case 3:
            min[i] = search.ivs[i];
            max[i] = 31;
        }
    }
    int minHp = min[0];
    int minAtk = min[1];
    int minDef = min[2];
    int maxHp = max[0];
    int maxAtk = max[1];
    int maxDef = max[2];
    int srcSpe = search.ivs[3];
    int srcSpA = search.ivs[4];
    int srcSpD = search.ivs[5];
    int hpSpe = srcSpe & 3;
    int hpSpA = srcSpA & 3;
    int hpSpD = srcSpD & 3;
    int cndHp = search.conditions[0];
    int cndAtk = search.conditions[1];
    int cndDef = search.conditions[2];
    int cndSpe = search.conditions[3];
    int cndSpA = search.conditions[4];
    int cndSpD = search.conditions[5];
    bool hgss = search.hgss;
    bool shiny = search.shiny;
    quint16 tid = search.trainerId;
    quint16 sid = search.secretId;
    int srcNature = search.nature;
    int srcAbility = search.ability;
    int srcHpType = search.hpType;
    int srcHpPower = search.hpPower;
    int year = search.year;
    quint16 minDelay = search.minDelay + year;
    quint16 maxDelay = search.maxDelay + year;
    int oddEvenDelay = search.oddEvenDelay;
    minDelay = (minDelay & 0xFFF7) | (search.oddEvenDelay & 1);
    maxDelay = (maxDelay & 0xFFF7) | (search.oddEvenDelay & 1);
    quint32 maxMainFrame = search.maxMainFrame;
    quint32 maxAltFrame = search.maxAltFrame;
    bool useSynch = search.useSynch;
    bool useEncSlots = search.useEncSlots;
    bool encSlots[34];
    for (int i = 0; i < 34; ++i) {
        encSlots[i] = search.encSlots[i];
    }
    int numEncSlots = hgss ? (34) : (32);
    int rndHpPower;
    int rndHpType;
    quint32 seed;
    quint32 baseSeed;
    quint32 mSeed;
    quint32 frame;
    quint32 pid;
    quint32 altFrame;
    quint32 altFrameTemp;
    bool foundSlot;
    int progress;
    Spread spread;
    EncounterFrames encFrames;
    spread.trainerId = tid;
    spread.secretId = sid;
    quint16 seedTop;
    quint16 seedBottom;
    quint32 nextSeed;
    quint16 nextRnd;
    int spe;
    int spa;
    int spd;
    bool nextSpread;
    progress = 0;
    for (int hp = maxHp; hp >= minHp; --hp) {
        for (int atk = maxAtk; atk >= minAtk; --atk) {
            ++progress;
            emit updateProgress(progress);
            for (int def = maxDef; def >= minDef; --def) {
                if (stop || abort) {
                    return false;
                }
                seedTop = hp | (atk << 5) | (def << 10);
                seedBottom = 0;
                while (true) {
                    ++seedBottom;
                    if (seedBottom == 0) {
                        break;
                    }
                    seed = seedBottom | (seedTop << 16);
                    nextSeed = seed * 0x41C64E6D + 0x6073;
                    nextRnd = nextSeed >> 16;
                    spe = nextRnd & 0x1F;
                    spa = (nextRnd >> 5) & 0x1F;
                    spd = (nextRnd >> 10) & 0x1F;
                    if ((((cndSpe == 0) && (spe >= srcSpe)) || ((cndSpe == 1) && (spe == srcSpe)) || ((cndSpe == 2) && (spe <= srcSpe)) || ((cndSpe == 3) && (spe >= srcSpe) && ((spe & 3) == hpSpe))) &&
                        (((cndSpA == 0) && (spa >= srcSpA)) || ((cndSpA == 1) && (spa == srcSpA)) || ((cndSpA == 2) && (spa <= srcSpA)) || ((cndSpA == 3) && (spa >= srcSpA) && ((spa & 3) == hpSpA))) &&
                        (((cndSpD == 0) && (spd >= srcSpD)) || ((cndSpD == 1) && (spd == srcSpD)) || ((cndSpD == 2) && (spd <= srcSpD)) || ((cndSpD == 3) && (spd >= srcSpD) && ((spd & 3) == hpSpD)))) {
                        if (srcHpPower > 30) {
                            rndHpPower = (((hp & 2) >> 1) | (atk & 2) | ((def & 2) << 1) | ((spe & 2) << 2) | ((spa & 2) << 3) | ((spd & 2) << 4));
                            rndHpPower = (((rndHpPower * 40) / 63) + 30);
                            if (rndHpPower < srcHpPower) {
                                continue; // Hidden Power is dependent on IVs
                            }
                        }
                        if (srcHpType >= 0) {
                            rndHpType = ((hp & 1) | ((atk & 1) << 1) | ((def & 1) << 2) | ((spe & 1) << 3) | ((spa & 1) << 4) | ((spd & 1) << 5));
                            rndHpType = ((rndHpType * 15) / 63);
                            if (rndHpType != srcHpType) {
                                continue; // Hidden Power is dependent on IVs
                            }
                        }
                        baseSeed = seed;
                        for (int topBit = 0; topBit < 2; ++topBit) {
                            baseSeed ^= (topBit << 31);
                            mSeed = baseSeed * 0x41C64E6D + 0x6073;
                            seed = baseSeed * 0xEEB9EB65 + 0xA3561A1;
                            pid = seed >> 16;
                            seed = seed * 0xEEB9EB65 + 0xA3561A1;
                            pid = (seed >> 16) | (pid << 16);
                            if (shiny && ((pid >> 16) ^ (pid & 0xFFFF) ^ tid ^ sid) >= 8) {
                                break; // Shininess is the same whether top bit is set or not
                            }
                            if (srcNature >= 0) {
                                if ((pid % 25) != srcNature) {
                                    continue; // Nature can differ depending on the top bit
                                }
                            }
                            if (srcAbility >= 0) {
                                if ((pid & 1) != srcAbility) {
                                    break; // Ability is the same whether top bit is set or not
                                }
                            }
                            seed = seed * 0xEEB9EB65 + 0xA3561A1;
                            frame = 1;
                            spread.pid = pid;
                            nextSpread = false;
                            while ((((seed >> 16) & 0xFF) >= 24) || ((seed & 0xFFFF) < minDelay) || ((seed & 0xFFFF) > maxDelay) || ((oddEvenDelay != 0) && ((((seed & 0xFFFF) - year) & 1) != (oddEvenDelay & 1)))) {
                                seed = seed * 0xEEB9EB65 + 0xA3561A1;
                                frame += 1;
                                if ((maxMainFrame > 0) && (frame > maxMainFrame)) {
                                    nextSpread = true;
                                    break;
                                }
                            }
                            if (nextSpread) {
                                break; // Frames are the same whether top bit is set or not
                            }
                            encFrames.initialize(mSeed, frame, search.hgss, spread.nature());
                            if (useEncSlots) {
                                encFrames.generateAll();
                            } else {
                                encFrames.findMinFrames();
                            }
                            if (maxAltFrame > 0) {
                                if (useSynch) {
                                    altFrame = encFrames.getMinFrame(true);
                                    altFrameTemp = encFrames.getMinFrame(false);
                                    if ((altFrameTemp > 0) && (altFrameTemp < altFrame)) {
                                        altFrame = altFrameTemp;
                                    }
                                } else {
                                    altFrame = encFrames.getMinFrame(false);
                                }
                                if ((altFrame == 0) || (altFrame > maxAltFrame)) {
                                    continue; // Method J/K frames can differ depending on the top bit
                                }
                            }
                            if (useEncSlots) {
                                altFrame = 0;
                                foundSlot = false;
                                for (int i = 0; i < numEncSlots; ++i) {
                                    if (!encSlots[i]) {
                                        continue;
                                    }
                                    if (useSynch) {
                                        altFrame = encFrames.getEncFrame(i, true);
                                    }
                                    altFrameTemp = encFrames.getEncFrame(i, false);
                                    if ((altFrameTemp > 0) || (altFrame > 0)) {
                                        foundSlot = true;
                                        break;
                                    }
                                }
                                if (!foundSlot) {
                                    continue; // Encounter slots can differ depending on the top bit
                                }
                            }
                            spread.ivs[0] = hp;
                            spread.ivs[1] = atk;
                            spread.ivs[2] = def;
                            spread.ivs[3] = spe;
                            spread.ivs[4] = spa;
                            spread.ivs[5] = spd;
                            spread.mainFrame = frame;
                            spread.mSeed = mSeed;
                            spread.seed = seed;
                            spread.altFrameSynch = encFrames.getMinFrame(true);
                            spread.altFrameNoSynch = encFrames.getMinFrame(false);
                            emit foundSpread(spread);
                        }
                    }
                }
                if (cndDef == 3) {
                    def += 3;
                }
            }
            if (cndAtk == 3) {
                atk += 3;
            }
        }
        if (cndHp == 3) {
            hp += 3;
        }
    }
    return true;
}

bool SearchThread::narrowSearchBottom()
{
    int min[3];
    int max[3];
    int searchIv;
    for (int i = 0; i < 3; ++i) {
        searchIv = search.ivs[i + 3];
        switch (search.conditions[i + 3]) {
        case 0:
            min[i] = searchIv;
            max[i] = 31;
            break;
        case 1:
            min[i] = searchIv;
            max[i] = searchIv;
            break;
        case 2:
            min[i] = 0;
            max[i] = searchIv;
            break;
        case 3:
            min[i] = searchIv;
            max[i] = 31;
        }
    }
    int minSpe = min[0];
    int minSpA = min[1];
    int minSpD = min[2];
    int maxSpe = max[0];
    int maxSpA = max[1];
    int maxSpD = max[2];
    int srcHp = search.ivs[0];
    int srcAtk = search.ivs[1];
    int srcDef = search.ivs[2];
    int hpHp = srcHp & 3;
    int hpAtk = srcAtk & 3;
    int hpDef = srcDef & 3;
    int cndHp = search.conditions[0];
    int cndAtk = search.conditions[1];
    int cndDef = search.conditions[2];
    int cndSpe = search.conditions[3];
    int cndSpA = search.conditions[4];
    int cndSpD = search.conditions[5];
    bool hgss = search.hgss;
    bool shiny = search.shiny;
    quint16 tid = search.trainerId;
    quint16 sid = search.secretId;
    int srcNature = search.nature;
    int srcAbility = search.ability;
    int srcHpType = search.hpType;
    int srcHpPower = search.hpPower;
    int year = search.year;
    quint16 minDelay = search.minDelay + year;
    quint16 maxDelay = search.maxDelay + year;
    int oddEvenDelay = search.oddEvenDelay;
    minDelay = (minDelay & 0xFFF7) | (search.oddEvenDelay & 1);
    maxDelay = (maxDelay & 0xFFF7) | (search.oddEvenDelay & 1);
    quint32 maxMainFrame = search.maxMainFrame;
    quint32 maxAltFrame = search.maxAltFrame;
    bool useSynch = search.useSynch;
    bool useEncSlots = search.useEncSlots;
    bool encSlots[34];
    for (int i = 0; i < 34; ++i) {
        encSlots[i] = search.encSlots[i];
    }
    int numEncSlots = hgss ? (34) : (32);
    int rndHpPower;
    int rndHpType;
    quint32 seed;
    quint32 baseSeed;
    quint32 mSeed;
    quint32 frame;
    quint32 pid;
    quint32 altFrame;
    quint32 altFrameTemp;
    bool foundSlot;
    int progress;
    Spread spread;
    EncounterFrames encFrames;
    spread.trainerId = tid;
    spread.secretId = sid;
    quint16 seedTop;
    quint16 seedBottom;
    quint16 rnd;
    int hp;
    int atk;
    int def;
    bool nextSpread;
    progress = 0;
    for (int spa = maxSpA; spa >= minSpA; --spa) {
        for (int spd = maxSpD; spd >= minSpD; --spd) {
            ++progress;
            emit updateProgress(progress);
            for (int spe = maxSpe; spe >= minSpe; --spe) {
                if (stop || abort) {
                    return false;
                }
                seedTop = spe | (spa << 5) | (spd << 10);
                seedBottom = 0;
                while (true) {
                    ++seedBottom;
                    if (seedBottom == 0) {
                        break;
                    }
                    seed = seedBottom | (seedTop << 16);
                    seed = seed * 0xEEB9EB65 + 0xA3561A1;
                    rnd = seed >> 16;
                    hp = rnd & 0x1F;
                    atk = (rnd >> 5) & 0x1F;
                    def = (rnd >> 10) & 0x1F;
                    if ((((cndHp == 0) && (hp >= srcHp)) || ((cndHp == 1) && (hp == srcHp)) || ((cndHp == 2) && (hp <= srcHp)) || ((cndHp == 3) && (hp >= srcHp) && ((hp & 3) == hpHp))) &&
                        (((cndAtk == 0) && (atk >= srcAtk)) || ((cndAtk == 1) && (atk == srcAtk)) || ((cndAtk == 2) && (atk <= srcAtk)) || ((cndAtk == 3) && (atk >= srcAtk) && ((atk & 3) == hpAtk))) &&
                        (((cndDef == 0) && (def >= srcDef)) || ((cndDef == 1) && (def == srcDef)) || ((cndDef == 2) && (def <= srcDef)) || ((cndDef == 3) && (def >= srcDef) && ((def & 3) == hpDef)))) {
                        if (srcHpPower > 30) {
                            rndHpPower = (((hp & 2) >> 1) | (atk & 2) | ((def & 2) << 1) | ((spe & 2) << 2) | ((spa & 2) << 3) | ((spd & 2) << 4));
                            rndHpPower = (((rndHpPower * 40) / 63) + 30);
                            if (rndHpPower < srcHpPower) {
                                continue; // Hidden Power is dependent on IVs
                            }
                        }
                        if (srcHpType >= 0) {
                            rndHpType = ((hp & 1) | ((atk & 1) << 1) | ((def & 1) << 2) | ((spe & 1) << 3) | ((spa & 1) << 4) | ((spd & 1) << 5));
                            rndHpType = ((rndHpType * 15) / 63);
                            if (rndHpType != srcHpType) {
                                continue; // Hidden Power is dependent on IVs
                            }
                        }
                        baseSeed = seed;
                        for (int topBit = 0; topBit < 2; ++topBit) {
                            baseSeed ^= (topBit << 31);
                            mSeed = baseSeed * 0x41C64E6D + 0x6073;
                            seed = baseSeed * 0xEEB9EB65 + 0xA3561A1;
                            pid = seed >> 16;
                            seed = seed * 0xEEB9EB65 + 0xA3561A1;
                            pid = (seed >> 16) | (pid << 16);
                            if (shiny && ((pid >> 16) ^ (pid & 0xFFFF) ^ tid ^ sid) >= 8) {
                                break; // Shininess is the same whether top bit is set or not
                            }
                            if (srcNature >= 0) {
                                if ((pid % 25) != srcNature) {
                                    continue; // Nature can differ depending on the top bit
                                }
                            }
                            if (srcAbility >= 0) {
                                if ((pid & 1) != srcAbility) {
                                    break; // Ability is the same whether top bit is set or not
                                }
                            }
                            seed = seed * 0xEEB9EB65 + 0xA3561A1;
                            frame = 1;
                            spread.pid = pid;
                            nextSpread = false;
                            while ((((seed >> 16) & 0xFF) >= 24) || ((seed & 0xFFFF) < minDelay) || ((seed & 0xFFFF) > maxDelay) || ((oddEvenDelay != 0) && ((((seed & 0xFFFF) - year) & 1) != (oddEvenDelay & 1)))) {
                                seed = seed * 0xEEB9EB65 + 0xA3561A1;
                                frame += 1;
                                if ((maxMainFrame > 0) && (frame > maxMainFrame)) {
                                    nextSpread = true;
                                    break;
                                }
                            }
                            if (nextSpread) {
                                break; // Frames are the same whether top bit is set or not
                            }
                            encFrames.initialize(mSeed, frame, search.hgss, spread.nature());
                            if (useEncSlots) {
                                encFrames.generateAll();
                            } else {
                                encFrames.findMinFrames();
                            }
                            if (maxAltFrame > 0) {
                                if (useSynch) {
                                    altFrame = encFrames.getMinFrame(true);
                                    altFrameTemp = encFrames.getMinFrame(false);
                                    if ((altFrameTemp > 0) && (altFrameTemp < altFrame)) {
                                        altFrame = altFrameTemp;
                                    }
                                } else {
                                    altFrame = encFrames.getMinFrame(false);
                                }
                                if ((altFrame == 0) || (altFrame > maxAltFrame)) {
                                    continue; // Method J/K frames can differ depending on the top bit
                                }
                            }
                            if (useEncSlots) {
                                altFrame = 0;
                                foundSlot = false;
                                for (int i = 0; i < numEncSlots; ++i) {
                                    if (!encSlots[i]) {
                                        continue;
                                    }
                                    if (useSynch) {
                                        altFrame = encFrames.getEncFrame(i, true);
                                    }
                                    altFrameTemp = encFrames.getEncFrame(i, false);
                                    if ((altFrameTemp > 0) || (altFrame > 0)) {
                                        foundSlot = true;
                                        break;
                                    }
                                }
                                if (!foundSlot) {
                                    continue; // Encounter slots can differ depending on the top bit
                                }
                            }
                            spread.ivs[0] = hp;
                            spread.ivs[1] = atk;
                            spread.ivs[2] = def;
                            spread.ivs[3] = spe;
                            spread.ivs[4] = spa;
                            spread.ivs[5] = spd;
                            spread.mainFrame = frame;
                            spread.mSeed = mSeed;
                            spread.seed = seed;
                            spread.altFrameSynch = encFrames.getMinFrame(true);
                            spread.altFrameNoSynch = encFrames.getMinFrame(false);
                            emit foundSpread(spread);
                        }
                    }
                }
                if (cndSpe == 3) {
                    spe += 3;
                }
            }
            if (cndSpD == 3) {
                spd += 3;
            }
        }
        if (cndSpA == 3) {
            spa += 3;
        }
    }
    return true;
}
