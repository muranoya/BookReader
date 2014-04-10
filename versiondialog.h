#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include "applicationinfo.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

class VersionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VersionDialog(QWidget *parent = 0);
    ~VersionDialog();

private slots:
    void button_close();

private:
    QVBoxLayout *layout;
    QLabel *label;
    QDialogButtonBox *button;
};

#endif // VERSIONDIALOG_H
