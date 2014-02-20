#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include "ApplicationInfo.h"
#include <QDialog>

namespace Ui
{
    class VersionDialog;
}

class VersionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VersionDialog(QWidget *parent = 0);
    ~VersionDialog();

    void showVersion();

private:
    Ui::VersionDialog *ui;
};

#endif // VERSIONDIALOG_H
