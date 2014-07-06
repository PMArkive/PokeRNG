#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "spreadsortmodel.h"
#include "spreadtablemodel.h"
#include "spreadinfo.h"
#include "encslotdialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDate>

const QStringList Spread::NatureNames = QString("Hardy,Lonely,Brave,Adamant,Naughty,"
                                                "Bold,Docile,Relaxed,Impish,Lax,"
                                                "Timid,Hasty,Serious,Jolly,Naive,"
                                                "Modest,Mild,Quiet,Bashful,Rash,"
                                                "Calm,Gentle,Sassy,Careful,Quirky").split(',');

const QStringList Spread::TypeNames = QString("Fighting,Flying,Poison,Ground,Rock,"
                                              "Bug,Ghost,Steel,Fire,Water,Grass,"
                                              "Electric,Psychic,Ice,Dragon,Dark").split(',');

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    spreadModel = new SpreadTableModel;
    proxyModel = new SpreadSortModel;
    spreadInfo = new SpreadInfo(this);
    spreadInfo->hide();
    encSlotEdit = new EncSlotDialog(this);
    encSlotEdit->hide();
    proxyModel->setSourceModel(spreadModel);
    proxyModel->setDynamicSortFilter(true);
    ui->setupUi(this);
    ui->results->setModel(proxyModel);
    ui->results->resizeColumnsToContents();
    ui->results->setColumnWidth(0, 83);
    ui->results->setColumnWidth(9, 83);
    hgss = false;
    qRegisterMetaType<Spread>("Spread");
    connect(ui->modeDppt, SIGNAL(toggled(bool)), this, SLOT(changeModeDppt(bool)));
    connect(ui->modeHgss, SIGNAL(toggled(bool)), this, SLOT(changeModeHgss(bool)));
    connect(ui->startSearch, SIGNAL(clicked()), this, SLOT(startSearch()));
    connect(ui->stopSearch, SIGNAL(clicked()), this, SLOT(stopSearch()));
    connect(ui->saveToTxt, SIGNAL(clicked()), this, SLOT(saveToTxt()));
    connect(ui->saveToCsv, SIGNAL(clicked()), this, SLOT(saveToCsv()));
    connect(&searchThread, SIGNAL(updateProgress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(&searchThread, SIGNAL(setMaxProgress(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(&searchThread, SIGNAL(foundSpread(Spread)), this, SLOT(foundSpread(Spread)));
    connect(&searchThread, SIGNAL(finished()), this, SLOT(finished()));
    connect(ui->results, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showSpreadInfo(QModelIndex)));
    connect(spreadModel, SIGNAL(spreadsAdded(QModelIndex,QModelIndex)), ui->results, SLOT(dataChanged(QModelIndex,QModelIndex)));
    connect(ui->encSlots, SIGNAL(toggled(bool)), ui->encSlotsEdit, SLOT(setEnabled(bool)));
    connect(ui->encSlots, SIGNAL(toggled(bool)), this, SLOT(refreshUseSynch()));
    connect(ui->maxAltFrame, SIGNAL(valueChanged(int)), this, SLOT(refreshUseSynch()));
    connect(ui->encSlotsEdit, SIGNAL(clicked()), this, SLOT(editEncSlots()));
    connect(ui->results, SIGNAL(spreadInfoSignal(QModelIndex)), this, SLOT(showSpreadInfo(QModelIndex)));
    ui->year->setValue(QDate::currentDate().year());
    changeModeDppt(true);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::changeModeDppt(bool enable)
{
    if (!enable) {
        return;
    }
    hgss = false;
    ui->minDelay->setValue(590 - (ui->year->value() - 2000));
    ui->maxDelay->setValue(750 - (ui->year->value() - 2000));
    ui->maxAltFrameLabel->setText(tr("Max. MJ Frame"));
}

void MainWidget::changeModeHgss(bool enable)
{
    if (!enable) {
        return;
    }
    hgss = true;
    ui->minDelay->setValue(490 - (ui->year->value() - 2000));
    ui->maxDelay->setValue(650 - (ui->year->value() - 2000));
    ui->maxAltFrameLabel->setText(tr("Max. MK Frame"));
}

void MainWidget::startSearch()
{
    spreads.clear();
    spreadModel->clear();
    ui->progressBar->setEnabled(true);
    ui->stopSearch->setEnabled(true);
    ui->startSearch->setEnabled(false);
    ui->saveToTxt->setEnabled(false);
    ui->saveToCsv->setEnabled(false);
    search.narrow = ui->modeNarrow->isChecked();
    search.ivs[0] = ui->hp->value();
    search.ivs[1] = ui->attack->value();
    search.ivs[2] = ui->defense->value();
    search.ivs[3] = ui->speed->value();
    search.ivs[4] = ui->spAtk->value();
    search.ivs[5] = ui->spDef->value();
    search.conditions[0] = SpreadSearch::Condition(ui->hpCondition->currentIndex());
    search.conditions[1] = SpreadSearch::Condition(ui->attackCondition->currentIndex());
    search.conditions[2] = SpreadSearch::Condition(ui->defenseCondition->currentIndex());
    search.conditions[3] = SpreadSearch::Condition(ui->speedCondition->currentIndex());
    search.conditions[4] = SpreadSearch::Condition(ui->spAtkCondition->currentIndex());
    search.conditions[5] = SpreadSearch::Condition(ui->spDefCondition->currentIndex());
    search.shiny = ui->shiny->isChecked();
    search.trainerId = ui->trainerId->value();
    search.secretId = ui->secretId->value();
    search.nature = ui->nature->currentIndex() - 1;
    search.ability = ui->ability->currentIndex() - 1;
    search.hpType = ui->hpType->currentIndex() - 1;
    search.hpPower = ui->hpPower->value();
    search.minDelay = ui->minDelay->value();
    search.maxDelay = ui->maxDelay->value();
    search.oddEvenDelay = ui->oddEvenDelay->currentIndex();
    search.year = ui->year->value() - 2000;
    search.maxMainFrame = ui->maxMainFrame->value();
    search.maxAltFrame = ui->maxAltFrame->value();
    search.useSynch = ui->useSynch->isChecked();
    bool useEncSlots = false;
    for (int i = 0; i < 34; ++i) {
        search.encSlots[i] = encSlots[i];
        if (encSlots[i]) {
            useEncSlots = true;
        }
    }
    search.useEncSlots = (ui->encSlots->isChecked() && useEncSlots);
    search.hgss = hgss;
    ui->progressBar->setValue(0);
    spreadModel->setHgss(hgss);
    spreadModel->setYear(ui->year->value() - 2000);
    searchThread.startSearch(search);
}

void MainWidget::stopSearch()
{
    searchThread.stopSearch();
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(false);
    ui->stopSearch->setEnabled(false);
    ui->startSearch->setEnabled(true);
    if (spreads.size() > 0) {
        ui->saveToTxt->setEnabled(true);
        ui->saveToCsv->setEnabled(true);
    }
}

void MainWidget::foundSpread(Spread spread)
{
    spreadModel->addSpread(spread);
    spreads.append(spread);
}

void MainWidget::finished()
{
    int numSpreads = spreadModel->rowCount(QModelIndex());
    QMessageBox::information(this, tr("Finished"), tr("Finished searching all spreads!\n%1 spread%2 found!").arg(numSpreads).arg((numSpreads == 1) ? "" : "s"));
    stopSearch();
}

void MainWidget::resizeEvent(QResizeEvent *)
{
    ui->gridLayoutWidget->resize(width() - 20, height() - 20);
    ui->horizontalLayoutWidget->resize(ui->groupBox->width() - 8, ui->horizontalLayoutWidget->height());
    ui->horizontalLayoutWidget_2->resize(ui->groupBox_2->width() - 8, ui->horizontalLayoutWidget_2->height());
}

void MainWidget::saveToTxt()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save to .txt..."), QString(), tr("Text files (*.txt)"));
    if (filename.isNull() || filename.isEmpty()) {
        return;
    }
    QFile file(filename);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream stream(&file);
    stream << "Search for: ";
    if (search.shiny) {
        stream << "Shiny (TID: " << search.trainerId << " SID: " << search.secretId << "), ";
    }
    if (search.nature >= 0) {
        stream << Spread::NatureNames[search.nature] << " nature, ";
    }
    if (search.ability >= 0) {
        stream << "Ability " << search.ability << ", ";
    }
    if ((search.hpPower > 30) || (search.hpType >= 0)) {
        stream << "Hidden Power " << Spread::TypeNames[search.hpType] << " " << search.hpPower << "+, ";
    }
    stream << "HP: " << ui->hpCondition->itemText(search.conditions[0]) << " " << search.ivs[0];
    stream << ", Atk: " << ui->attackCondition->itemText(search.conditions[1]) << " " << search.ivs[1];
    stream << ", Def: " << ui->defenseCondition->itemText(search.conditions[2]) << " " << search.ivs[2];
    stream << ", SpA: " << ui->spAtkCondition->itemText(search.conditions[4]) << " " << search.ivs[4];
    stream << ", SpD: " << ui->spDefCondition->itemText(search.conditions[5]) << " " << search.ivs[5];
    stream << ", Spe: " << ui->speedCondition->itemText(search.conditions[3]) << " " << search.ivs[3];
    stream << ", Min. delay: " << search.minDelay << ", Max. delay: " << search.maxDelay << ", Year: " << search.year + 2000;
    if (search.oddEvenDelay == 1) {
        stream << " (odd delays only)";
    } else if (search.oddEvenDelay == 2) {
        stream << " (even delays only)";
    }
    if (search.maxMainFrame > 0) {
        stream << ", Max. Method 1 frame: " << search.maxMainFrame;
    }
    if (search.maxAltFrame > 0) {
        if (search.hgss) {
            stream << ", Max. Method K frame: " << search.maxAltFrame;
        } else {
            stream << ", Max. Method J frame: " << search.maxAltFrame;
        }
    }
    if (search.useEncSlots) {
        stream << ", Encounter Slots ";
        if (search.hgss) {
            stream << "(Method K): ";
        } else {
            stream << "(Method J): ";
        }
        bool started = false;
        QStringList types;
        types.append("Land");
        types.append("Water");
        types.append("Old Rod");
        types.append("Good Rod");
        types.append("Super Rod");
        types.append("Rock Smash");
        QVector<QVector<int> > usedSlots;
        usedSlots.resize(6);
        for (int i = 0; i < 12; ++i) {
            if (search.encSlots[i]) {
                usedSlots[0].append(i);
            }
        }
        for (int i = 0; i < 5; ++i) {
            if (search.encSlots[12 + i]) {
                usedSlots[1].append(i);
            }
            if (search.encSlots[17 + i]) {
                usedSlots[2].append(i);
            }
            if (search.encSlots[22 + i]) {
                usedSlots[3].append(i);
            }
            if (search.encSlots[27 + i]) {
                usedSlots[4].append(i);
            }
        }
        if (search.hgss) {
            for (int i = 0; i < 2; ++i) {
                if (search.encSlots[32 + i]) {
                    usedSlots[5].append(i);
                }
            }
        }
        for (int i = 0; i < 6; ++i) {
            if (!usedSlots[i].isEmpty()) {
                if (started) {
                    stream << "; ";
                }
                stream << types[i] << " ";
                started = true;
                for (int j = 0; j < usedSlots[i].size(); ++j) {
                    stream << usedSlots[i][j];
                    if (j < (usedSlots[i].size() - 1)) {
                        stream << ", ";
                    }
                }
            }
        }
    }
    if ((search.maxAltFrame) > 0 || (search.useEncSlots)) {
        if (search.useSynch) {
            stream << " (with synch)";
        } else {
            stream << " (without synch)";
        }
    }
    stream << endl;
    for (int i = 0; i < spreads.size(); ++i) {
        stream << spreadModel->rowString(proxyModel->mapToSource(proxyModel->index(i, 0)).row()) << endl;
    }
}

void MainWidget::saveToCsv()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save to .csv..."), QString(), tr("Comma-separated values (*.csv)"));
    if (filename.isNull() || filename.isEmpty()) {
        return;
    }
    QFile file(filename);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream stream(&file);
    Spread spread;
    QString pid;
    QString seed;
    for (int i = 0; i < spreads.size(); ++i) {
        spread = spreads[proxyModel->mapToSource(proxyModel->index(i, 0)).row()];
        pid = QString("%1").arg(spread.pid, 8, 16, QChar('0')).toUpper();
        seed = QString("%1").arg(spread.seed, 8, 16, QChar('0')).toUpper();
        stream << pid << "," << Spread::NatureNames[spread.nature()] << "," << spread.ability() << ",";
        stream << spread.ivs[0] << "," << spread.ivs[1] << "," << spread.ivs[2] << "," << spread.ivs[4] << "," << spread.ivs[5] << "," << spread.ivs[3] << ",";
        stream << seed << "," << (spread.delay() - search.year) << "," << spread.mainFrame << "," << spread.altFrameSynch << "," << spread.altFrameNoSynch << ",";
        stream << Spread::TypeNames[spread.hpType()] << "," << spread.hpPower() << endl;
    }
}

void MainWidget::showSpreadInfo(QModelIndex index)
{
    spreadInfo->loadSpread(spreads[proxyModel->mapToSource(index).row()], hgss);
    spreadInfo->show();
}

void MainWidget::editEncSlots()
{
    encSlotEdit->setHgss(hgss);
    for (int i = 0; i < 34; ++i) {
        encSlotEdit->setSlotEnabled(i, search.encSlots[i]);
    }
    if (encSlotEdit->exec() == QDialog::Accepted) {
        for (int i = 0; i < 34; ++i) {
            encSlots[i] = encSlotEdit->getSlotEnabled(i);
        }
    }
}

void MainWidget::refreshUseSynch()
{
    ui->useSynch->setEnabled(ui->encSlots->isChecked() || (ui->maxAltFrame->value() > 0));
}
