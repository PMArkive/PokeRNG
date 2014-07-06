#ifndef SPREADINFO_H
#define SPREADINFO_H

#include <QDialog>
#include <QList>
#include "spread.h"
#include "encounterframes.h"

namespace Ui {
    class SpreadInfo;
}

class SpreadInfo : public QDialog
{
    Q_OBJECT

public:
    explicit SpreadInfo(QWidget *parent = 0);
    ~SpreadInfo();
    void loadSpread(Spread newSpread, bool hgssEnabled);
    void setHgss(bool enabled);

public slots:
    void toggleHgss();
    void findTimes();
    void displayFrames();

protected:
    void resizeEvent(QResizeEvent *);

private:
    EncounterFrames methodJFrames;
    EncounterFrames methodKFrames;
    Ui::SpreadInfo *ui;
    QString coinFlips;
    QString elmCalls;
    bool hgss;
    Spread spread;
};

#endif // SPREADINFO_H
