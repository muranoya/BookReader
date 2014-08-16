#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent,
            Qt::Sheet |
            Qt::WindowTitleHint |
            Qt::WindowCloseButtonHint)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("設定"));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
