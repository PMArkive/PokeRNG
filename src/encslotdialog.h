#ifndef ENCSLOTDIALOG_H
#define ENCSLOTDIALOG_H

#include <QDialog>

class QCheckBox;

namespace Ui {
    class EncSlotDialog;
}

class EncSlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EncSlotDialog(QWidget *parent = 0);
    ~EncSlotDialog();
    void setHgss(bool enabled);
    void setSlotEnabled(int slot, bool enabled);
    bool getSlotEnabled(int slot);

private:
    QCheckBox* slotBox(int slot);
    Ui::EncSlotDialog *ui;
    bool hgss;
};

#endif // ENCSLOTDIALOG_H
