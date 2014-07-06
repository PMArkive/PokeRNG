#include "encslotdialog.h"
#include "ui_encslotdialog.h"

EncSlotDialog::EncSlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EncSlotDialog)
{
    ui->setupUi(this);
}

EncSlotDialog::~EncSlotDialog()
{
    delete ui;
}

void EncSlotDialog::setHgss(bool enabled)
{
    hgss = enabled;
    ui->rockSmash0->setEnabled(enabled);
    ui->rockSmash1->setEnabled(enabled);
}

void EncSlotDialog::setSlotEnabled(int slot, bool enabled)
{
    QCheckBox* box = slotBox(slot);
    if (box == 0) {
        return;
    }
    box->setChecked(enabled);
}

bool EncSlotDialog::getSlotEnabled(int slot)
{
    QCheckBox* box = slotBox(slot);
    if (box == 0) {
        return false;
    }
    return box->isChecked();
}

QCheckBox* EncSlotDialog::slotBox(int slot)
{
    switch (slot) {
    case 0:
        return ui->land0;
    case 1:
        return ui->land1;
    case 2:
        return ui->land2;
    case 3:
        return ui->land3;
    case 4:
        return ui->land4;
    case 5:
        return ui->land5;
    case 6:
        return ui->land6;
    case 7:
        return ui->land7;
    case 8:
        return ui->land8;
    case 9:
        return ui->land9;
    case 10:
        return ui->land10;
    case 11:
        return ui->land11;
    case 12:
        return ui->water0;
    case 13:
        return ui->water1;
    case 14:
        return ui->water2;
    case 15:
        return ui->water3;
    case 16:
        return ui->water4;
    case 17:
        return ui->oldRod0;
    case 18:
        return ui->oldRod1;
    case 19:
        return ui->oldRod2;
    case 20:
        return ui->oldRod3;
    case 21:
        return ui->oldRod4;
    case 22:
        return ui->goodRod0;
    case 23:
        return ui->goodRod1;
    case 24:
        return ui->goodRod2;
    case 25:
        return ui->goodRod3;
    case 26:
        return ui->goodRod4;
    case 27:
        return ui->superRod0;
    case 28:
        return ui->superRod1;
    case 29:
        return ui->superRod2;
    case 30:
        return ui->superRod3;
    case 31:
        return ui->superRod4;
    case 32:
        return ui->rockSmash0;
    case 33:
        return ui->rockSmash1;
    default:
        return 0;
    }
}
