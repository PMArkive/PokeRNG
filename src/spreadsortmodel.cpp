#include "spreadsortmodel.h"

SpreadSortModel::SpreadSortModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool SpreadSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    if (leftData.type() != rightData.type()) {
        return ((leftData.type() == QVariant::Int) == (sortOrder() == Qt::AscendingOrder));
    }
    return QSortFilterProxyModel::lessThan(left, right);
}
