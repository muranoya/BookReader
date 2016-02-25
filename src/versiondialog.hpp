#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include "applicationinfo.hpp"

#include <QDialog>
#include <QGridLayout>
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
    QGridLayout *layout;
    QLabel *nameDesc, *nameLabel;
    QLabel *verDesc,  *verLabel;
    QLabel *qtDesc,   *qtLabel;
    QDialogButtonBox *button;
};

#endif // VERSIONDIALOG_H
