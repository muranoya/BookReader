#ifndef SETTINGROTATEDIALOG_H
#define SETTINGROTATEDIALOG_H

#include <QDialog>

namespace Ui
{
    class SettingRotateDialog;
}

class SettingRotateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingRotateDialog(QWidget *parent = 0);
    ~SettingRotateDialog();

    bool getRotate(qreal deg);
    qreal getValue();

private:
    qreal rotate;

    Ui::SettingRotateDialog *ui;
};

#endif // SETTINGROTATEDIALOG_H
