#include "spreadtablemodel.h"

SpreadTableModel::SpreadTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    hgss = false;
    spreadCount = 0;
}

void SpreadTableModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, spreadCount - 1);
    removeRows(0, spreadCount);
    spreads.clear();
    spreadCount = 0;
    endRemoveRows();
}

void SpreadTableModel::addSpread(Spread &spread)
{
    spreads.append(spread);
    fetchMore(QModelIndex());
}

bool SpreadTableModel::canFetchMore(const QModelIndex &parent) const
{
    return (spreadCount < spreads.size());
}

void SpreadTableModel::fetchMore(const QModelIndex &parent)
{
    int spreadsToFetch = qMin(100, spreads.size() - spreadCount);
    int first = spreadCount;
    int last = spreadCount + spreadsToFetch - 1;
    beginInsertRows(QModelIndex(), first, last);
    spreadCount += spreadsToFetch;
    endInsertRows();
    const QModelIndex& firstIndex = index(first, 0);
    const QModelIndex& lastIndex = index(last, 16);
    emit spreadsAdded(firstIndex, lastIndex);
}

QVariant SpreadTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if ((index.row() > spreads.size()) || (index.row() < 0)) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        const Spread& spread = spreads[index.row()];
        switch (index.column()) {
        case 0:
            return QString("%1").arg(spread.pid, 8, 16, QChar('0')).toUpper();
        case 1:
            return Spread::NatureNames[spread.nature()];
        case 2:
            return spread.ability();
        case 3:
            return spread.ivs[0];
        case 4:
            return spread.ivs[1];
        case 5:
            return spread.ivs[2];
        case 6:
            return spread.ivs[4];
        case 7:
            return spread.ivs[5];
        case 8:
            return spread.ivs[3];
        case 9:
            return QString("%1").arg(spread.seed, 8, 16, QChar('0')).toUpper();
        case 10:
            return spread.delay() - year;
        case 11:
            return spread.mainFrame;
        case 12:
            if (spread.altFrameSynch > 0) {
                return spread.altFrameSynch;
            } else {
                return QString("None");
            }
        case 13:
            if (spread.altFrameNoSynch > 0) {
                return spread.altFrameNoSynch;
            } else {
                return QString("None");
            }
        case 14:
            return Spread::TypeNames[spread.hpType()];
        case 15:
            return spread.hpPower();
        }
    }
    return QVariant();
}

QVariant SpreadTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section >= 0 && section < 16 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("PID");
        case 1:
            return tr("Nature");
        case 2:
            return tr("Abi");
        case 3:
            return tr("HP");
        case 4:
            return tr("Atk");
        case 5:
            return tr("Def");
        case 6:
            return tr("SpA");
        case 7:
            return tr("SpD");
        case 8:
            return tr("Spe");
        case 9:
            return tr("Seed");
        case 10:
            return tr("Delay");
        case 11:
            return tr("Method 1");
        case 12:
            return hgss ? tr("K (synch)") :  tr("J (synch)");
        case 13:
            return hgss ? tr("K (no synch)") :  tr("J (no synch)");
        case 14:
            return tr("Hidden");
        case 15:
            return tr("Power");
        }
    }
    return QVariant();
}

int SpreadTableModel::rowCount(const QModelIndex &parent) const
{
    return spreadCount;
}

int SpreadTableModel::columnCount(const QModelIndex &parent) const
{
    return 16;
}

void SpreadTableModel::setHgss(bool enabled)
{
    hgss = enabled;
}

void SpreadTableModel::setYear(int value)
{
    year = value;
}

QString SpreadTableModel::rowString(int row) const
{
    Spread spread = spreads[row];
    QString string;
    QString pid = QString("%1").arg(spread.pid, 8, 16, QChar('0')).toUpper();
    QString seed = QString("%1").arg(spread.seed, 8, 16, QChar('0')).toUpper();
    QString altFrameType = hgss ? tr("K") : tr("J");
    QString altFrameSynch = (spread.altFrameSynch > 0) ? QString("%1").arg(spread.altFrameSynch) : tr("None");
    QString altFrameNoSynch = (spread.altFrameNoSynch > 0) ? QString("%1").arg(spread.altFrameNoSynch) : tr("None");
    string = tr("PID: %1 (%2, ability %3), IVs: %4/%5/%6/%7/%8/%9, seed: %10, delay: %11 (year: %12), method 1: %13, %14 (synch): %15, %14 (no synch): %16, HP: %17 %18")
            .arg(pid).arg(Spread::NatureNames[spread.nature()]).arg(spread.ability())
            .arg(spread.ivs[0]).arg(spread.ivs[1]).arg(spread.ivs[2]).arg(spread.ivs[4]).arg(spread.ivs[5]).arg(spread.ivs[3])
            .arg(seed).arg(spread.delay() - year).arg(year + 2000).arg(spread.mainFrame).arg(altFrameType).arg(altFrameSynch).arg(altFrameNoSynch)
            .arg(Spread::TypeNames[spread.hpType()]).arg(spread.hpPower());
    return string;
}
