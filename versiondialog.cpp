#include "versiondialog.h"
#include "ui_versiondialog.h"

VersionDialog::VersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionDialog)
{
    ui->setupUi(this);
}

VersionDialog::~VersionDialog()
{
    delete ui;
}

void VersionDialog::showVersion()
{
    setWindowTitle(tr("バージョン"));
    ui->label_software->setText(tr("ソフトウェア名"));
    ui->label_software_text->setText(QString(SOFTWARE_NAME));
    ui->label_version->setText(tr("バージョン"));
    ui->label_version_text->setText(QString(SOFTWARE_VERSION));
    exec();
}
