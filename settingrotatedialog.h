#ifndef SETTINGROTATEDIALOG_H
#define SETTINGROTATEDIALOG_H

#include <QDialog>

namespace Ui {
class SettingRotateDialog;
}

class SettingRotateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingRotateDialog(QWidget *parent = 0);
    ~SettingRotateDialog();

    qreal getRotate(qreal def);

private:
    Ui::SettingRotateDialog *ui;
};

#endif // SETTINGROTATEDIALOG_H
