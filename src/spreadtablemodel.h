#ifndef SPREADTABLEMODEL_H
#define SPREADTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QStringList>
#include "spread.h"

class SpreadTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SpreadTableModel(QObject *parent = 0);
    void clear();
    void addSpread(Spread& spread);
    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void setHgss(bool enabled);
    void setYear(int value);
    QString rowString(int row) const;

signals:
    void spreadsAdded(const QModelIndex& first, const QModelIndex& last);

public slots:

private:
    bool hgss;
    int year;
    QList<Spread> spreads;
    int spreadCount;
    QStringList natures;
    QStringList types;
};

#endif // SPREADTABLEMODEL_H
