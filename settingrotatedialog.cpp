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

qreal SettingRotateDialog::getRotate(qreal def)
{
    setWindowTitle(tr("角度の設定"));
    ui->spinBox->setValue((int)def);
    int ret = exec();
    if (ret == QDialog::Accepted)
    {
        return (qreal)ui->spinBox->value();
    }
    return def;
}
