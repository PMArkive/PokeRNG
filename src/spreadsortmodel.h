#ifndef SPREADSORTMODEL_H
#define SPREADSORTMODEL_H

#include <QSortFilterProxyModel>

class SpreadSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SpreadSortModel(QObject *parent = 0);

signals:

public slots:

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // SPREADSORTMODEL_H
