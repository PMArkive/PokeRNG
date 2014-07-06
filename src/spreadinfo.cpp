#include "spreadinfo.h"
#include "ui_spreadinfo.h"
#include "mersennetwister.h"
#include "pokemonrng.h"
#include <QDate>
#include <QTime>

SpreadInfo::SpreadInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpreadInfo)
{
    ui->setupUi(this);
    connect(ui->toggleHgss, SIGNAL(clicked()), this, SLOT(toggleHgss()));
    connect(ui->toggleHgss, SIGNAL(clicked()), this, SLOT(displayFrames()));
    connect(ui->year, SIGNAL(valueChanged(int)), this, SLOT(findTimes()));
    connect(ui->second, SIGNAL(valueChanged(int)), this, SLOT(findTimes()));
    ui->encounterTable->resizeColumnsToContents();
    ui->encounterTable->setColumnWidth(0, ui->encounterTable->columnWidth(0) + 20);
    ui->encounterTable->setColumnWidth(2, ui->encounterTable->columnWidth(2) + 8);
    ui->encounterTable->setColumnWidth(3, ui->encounterTable->columnWidth(3) + 8);
    ui->year->setValue(QDate::currentDate().year());
}

SpreadInfo::~SpreadInfo()
{
    delete ui;
}

void SpreadInfo::loadSpread(Spread newSpread, bool hgssEnabled)
{
    spread = newSpread;
    MersenneTwister twister(spread.seed);
    PokemonRNG rng;
    rng.setSeed(spread.seed);
    coinFlips = tr("Coin Flips: ");
    elmCalls = tr("Elm Calls: ");
    for (int i = 0; i < 10; i++) {
        if ((twister.getNext() & 1) == 0) {
            coinFlips.append("T");
        } else {
            coinFlips.append("H");
        }
        switch (rng.getRand() % 3) {
        case 0:
            elmCalls.append("E");
            break;
        case 1:
            elmCalls.append("K");
            break;
        case 2:
            elmCalls.append("P");
            break;
        }
        if (i < 9) {
            coinFlips.append(", ");
            elmCalls.append(", ");
        }
    }
    ui->seedLabel->setText(tr("Seed: 0x") + QString("%1").arg(spread.seed, 8, 16, QChar('0')).toUpper());
    ui->method1Label->setText(tr("Method 1 Frame: %1").arg(spread.mainFrame));
    ui->natureLabel->setText(tr("Nature: %1").arg(Spread::NatureNames[spread.nature()]));
    ui->ivLabel->setText(tr("IVs: %1/%2/%3/%4/%5/%6").arg(spread.ivs[0]).arg(spread.ivs[1]).arg(spread.ivs[2]).arg(spread.ivs[4]).arg(spread.ivs[5]).arg(spread.ivs[3]));
    methodJFrames.initialize(spread.mSeed, spread.mainFrame, hgss, spread.nature());
    methodKFrames.initialize(spread.mSeed, spread.mainFrame, hgss, spread.nature());
    setHgss(hgssEnabled);
    findTimes();
}

void SpreadInfo::setHgss(bool enabled)
{
    hgss = enabled;
    if (hgss) {
        ui->toggleHgss->setText(tr("Switch to DPPt mode"));
        ui->encounterTable->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("K (synch)")));
        ui->encounterTable->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("K (no synch)")));
        ui->seedCheckLabel->setText(elmCalls);
    } else {
        ui->toggleHgss->setText(tr("Switch to HGSS mode"));
        ui->encounterTable->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("J (synch)")));
        ui->encounterTable->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("J (no synch)")));
        ui->seedCheckLabel->setText(coinFlips);
    }
    displayFrames();
}

void SpreadInfo::toggleHgss()
{
    setHgss(!hgss);
}

void SpreadInfo::findTimes()
{
    ui->timesTable->clearContents();
    ui->timesTable->setRowCount(0);
    QDate date(ui->year->value(), 1, 1);
    QTime time;
    int nextYear = ui->year->value() + 1;
    int topByte = spread.seed >> 24;
    int hour = (spread.seed >> 16) & 0xFF;
    int second = ui->second->value();
    int delay = ((spread.seed & 0xFFFF) - (ui->year->value() - 2000)) & 0xFFFF;
    int minAdjust = second;
    int maxAdjust = second + 59;
    int base;
    int min;
    int max;
    int row = 0;
    while (date.year() < nextYear) {
        base = date.month() * date.day();
        min = (base + minAdjust) & 0xFF;
        max = (base + maxAdjust) & 0xFF;
        if (((topByte >= min) && (topByte <= max)) || ((min > max) && ((topByte >= min) || (topByte <= max)))) {
            time.setHMS(hour, (topByte - min) & 0xFF, second);
            ui->timesTable->insertRow(row);
            ui->timesTable->setItem(row, 0, new QTableWidgetItem(date.toString(Qt::SystemLocaleShortDate)));
            ui->timesTable->setItem(row, 1, new QTableWidgetItem(time.toString(Qt::TextDate)));
            ui->timesTable->setItem(row, 2, new QTableWidgetItem(tr("%1").arg(delay)));
            ++row;
        }
        date = date.addDays(1);
    }
}

void SpreadInfo::displayFrames()
{
    int row = 0;
    quint32 synchFrame;
    quint32 noSynchFrame;
    QString slotType;
    ui->encounterTable->clearContents();
    ui->encounterTable->setRowCount(0);
    EncounterFrames &frames = (hgss ? methodKFrames : methodJFrames);
    if (!frames.hasGenerated()) {
        frames.initialize(spread.mSeed, spread.mainFrame, hgss, spread.nature());
        frames.generateAll();
    }
    EncounterFrames::SlotType end = hgss ? EncounterFrames::RockSmash : EncounterFrames::SuperRod;
    for (EncounterFrames::SlotType t = EncounterFrames::Land; t <= end; t = EncounterFrames::SlotType(t + 1)) {
        for (int s = 0; s < 12; ++s) {
            if (t == EncounterFrames::RockSmash) {
                if (s >= 2) {
                    break;
                }
            } else if (t >= EncounterFrames::Water) {
                if (s >= 5) {
                    break;
                }
            }
            synchFrame = frames.getEncFrame(t, s, true);
            noSynchFrame = frames.getEncFrame(t, s, false);
            if ((synchFrame == 0) && (noSynchFrame == 0)) {
                continue;
            }
            switch (t) {
            case EncounterFrames::Land:
                slotType = tr("Land");
                break;
            case EncounterFrames::Water:
                slotType = tr("Water");
                break;
            case EncounterFrames::OldRod:
                slotType = tr("Old Rod");
                if (hgss) {
                    slotType.append(" (tentative)");
                }
                break;
            case EncounterFrames::GoodRod:
                slotType = tr("Good Rod");
                if (hgss) {
                    slotType.append(" (tentative)");
                }
                break;
            case EncounterFrames::SuperRod:
                slotType = tr("Super Rod");
                if (hgss) {
                    slotType.append(" (tentative)");
                }
                break;
            case EncounterFrames::RockSmash:
                slotType = tr("Rock Smash (tentative)");
                break;
            }
            ui->encounterTable->insertRow(row);
            ui->encounterTable->setItem(row, 0, new QTableWidgetItem(slotType));
            ui->encounterTable->setItem(row, 1, new QTableWidgetItem(tr("%1").arg(s)));
            if (synchFrame > 0) {
                ui->encounterTable->setItem(row, 2, new QTableWidgetItem(tr("%1").arg(synchFrame)));
            } else {
                ui->encounterTable->setItem(row, 2, new QTableWidgetItem(tr("None")));
            }
            if (noSynchFrame > 0) {
                ui->encounterTable->setItem(row, 3, new QTableWidgetItem(tr("%1").arg(noSynchFrame)));
            } else {
                ui->encounterTable->setItem(row, 3, new QTableWidgetItem(tr("None")));
            }
            ++row;
        }
    }
}

void SpreadInfo::resizeEvent(QResizeEvent *)
{
    ui->gridLayoutWidget->resize(width() - 20, height() - 60);
    ui->buttonBox->move(width() - 370, height() - 40);
}
