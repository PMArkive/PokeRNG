#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QList>
#include <QtWidgets/QWidget>
#include <QModelIndex>
#include "searchthread.h"
#include "spreadsearch.h"
#include "spread.h"

namespace Ui {
    class MainWidget;
}

class SpreadSortModel;
class SpreadTableModel;
class SpreadInfo;
class EncSlotDialog;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:
    void changeModeDppt(bool enable);
    void changeModeHgss(bool enable);
    void startSearch();
    void stopSearch();
    void foundSpread(Spread spread);
    void finished();
    void saveToTxt();
    void saveToCsv();
    void showSpreadInfo(QModelIndex index);
    void editEncSlots();
    void refreshUseSynch();

protected:
    void resizeEvent(QResizeEvent *);

private:
    SpreadSortModel* proxyModel;
    SpreadTableModel* spreadModel;
    SpreadInfo* spreadInfo;
    EncSlotDialog* encSlotEdit;
    bool encSlots[34];
    QList<Spread> spreads;
    Ui::MainWidget *ui;
    bool hgss;
    SearchThread searchThread;
    SpreadSearch search;
};

#endif // MAINWIDGET_H
