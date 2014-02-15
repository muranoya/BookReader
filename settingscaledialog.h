#ifndef SETTINGSCALEDIALOG_H
#define SETTINGSCALEDIALOG_H

#include <QDialog>

namespace Ui {
class SettingScaleDialog;
}

class SettingScaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingScaleDialog(QWidget *parent = 0);
    ~SettingScaleDialog();

    qreal getScale(qreal ori);

private:
    Ui::SettingScaleDialog *ui;
};

#endif // SETTINGSCALEDIALOG_H
