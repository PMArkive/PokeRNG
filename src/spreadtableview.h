#ifndef SPREADTABLEVIEW_H
#define SPREADTABLEVIEW_H

#include <QTableView>

class SpreadSortModel;

class SpreadTableView : public QTableView
{
    Q_OBJECT
public:
    explicit SpreadTableView(QWidget *parent = 0);

signals:
    void spreadInfoSignal(QModelIndex index);

public slots:
    void spreadInformation();
    void copyRows();
    void copyItem();
    void copySeed();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QPoint menuPos;
    QAction* spreadInformationAction;
    QAction* copyRowsAction;
    QAction* copyItemAction;
    QAction* copySeedAction;

};

#endif // SPREADTABLEVIEW_H
