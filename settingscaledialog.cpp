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

qreal SettingScaleDialog::getScale(qreal ori)
{
    this->setWindowTitle(tr("倍率の設定"));
    ui->spinBox->setValue((int)ori);
    int ret = this->exec();
    if (ret == QDialog::Accepted)
    {
        return (qreal)ui->spinBox->value();
    }
    return 1.0;
}
