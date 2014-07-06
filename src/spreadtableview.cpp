#include "spreadtableview.h"
#include "spreadsortmodel.h"
#include "spreadtablemodel.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

SpreadTableView::SpreadTableView(QWidget *parent) :
    QTableView(parent)
{
    spreadInformationAction = new QAction(tr("Spread &Information"), parent);
    connect(spreadInformationAction, SIGNAL(triggered()), this, SLOT(spreadInformation()));

    copyRowsAction = new QAction(tr("Copy &Row(s) to Clipboard"), parent);
    //copyRowsAction->setShortcuts(QKeySequence::Copy);
    connect(copyRowsAction, SIGNAL(triggered()), this, SLOT(copyRows()));

    copyItemAction = new QAction(tr("Copy &Item to Clipboard"), parent);
    connect(copyItemAction, SIGNAL(triggered()), this, SLOT(copyItem()));

    copySeedAction = new QAction(tr("Copy &Seed to Clipboard"), parent);
    connect(copySeedAction, SIGNAL(triggered()), this, SLOT(copySeed()));
}

void SpreadTableView::spreadInformation()
{
    emit spreadInfoSignal(indexAt(menuPos));
}

void SpreadTableView::copyRows()
{
    SpreadSortModel* proxy = qobject_cast<SpreadSortModel*>(model());
    SpreadTableModel* model = qobject_cast<SpreadTableModel*>(proxy->sourceModel());
    QString text;
    QModelIndexList indexes = selectedIndexes();
    QList<int> rows;
    int row;
    for (int i = 0; i < indexes.size(); ++i) {
        row = indexes[i].row();
        if (!rows.contains(row)) {
            rows.append(row);
        }
    }
    for (int i = 0; i < rows.size(); ++i) {
        text.append(model->rowString(proxy->mapToSource(proxy->index(rows[i], 0)).row()));
        if (i < (rows.size() - 1)) {
            text.append("\n");
        }
    }
    QApplication::clipboard()->setText(text);
}

void SpreadTableView::copyItem()
{
    SpreadSortModel* proxy = qobject_cast<SpreadSortModel*>(model());
    QApplication::clipboard()->setText(proxy->sourceModel()->data(proxy->mapToSource(indexAt(menuPos))).toString());
}

void SpreadTableView::copySeed()
{
    SpreadSortModel* proxy = qobject_cast<SpreadSortModel*>(model());
    int row = proxy->mapToSource(indexAt(menuPos)).row();
    QApplication::clipboard()->setText(proxy->sourceModel()->data(proxy->sourceModel()->index(row, 9)).toString());
}

void SpreadTableView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(spreadInformationAction);
    menu.addAction(copyRowsAction);
    menu.addAction(copyItemAction);
    menu.addAction(copySeedAction);
    menuPos = event->pos();
    menu.exec(event->globalPos());
}
