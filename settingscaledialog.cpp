#include "settingscaledialog.h"
#include "ui_settingscaledialog.h"

SettingScaleDialog::SettingScaleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingScaleDialog)
{
    ui->setupUi(this);
}

SettingScaleDialog::~SettingScaleDialog()
{
    delete ui;
}

bool SettingScaleDialog::getScale(qreal ori)
{
    setWindowTitle(tr("倍率の設定"));
    ui->doubleSpinBox->setValue(ori);
    if (exec() == QDialog::Accepted)
    {
        scale = (qreal)ui->doubleSpinBox->value();
        return true;
    }
    else
    {
        scale = ori;
        return false;
    }
}

qreal SettingScaleDialog::getValue()
{
    return scale;
}
