#include "settingrotatedialog.h"
#include "ui_settingrotatedialog.h"

SettingRotateDialog::SettingRotateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingRotateDialog)
{
    ui->setupUi(this);
}

SettingRotateDialog::~SettingRotateDialog()
{
    delete ui;
}

bool SettingRotateDialog::getRotate(qreal deg)
{
    setWindowTitle(tr("角度の設定"));
    ui->doubleSpinBox->setValue(deg);
    if (exec() == QDialog::Accepted)
    {
        rotate = (qreal)ui->doubleSpinBox->value();
        return true;
    }
    else
    {
        rotate = deg;
        return false;
    }
}

qreal SettingRotateDialog::getValue()
{
    return rotate;
}
