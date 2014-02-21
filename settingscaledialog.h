#ifndef SETTINGSCALEDIALOG_H
#define SETTINGSCALEDIALOG_H

#include <QDialog>

namespace Ui
{
    class SettingScaleDialog;
}

class SettingScaleDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingScaleDialog(QWidget *parent = 0);
    ~SettingScaleDialog();

    bool getScale(qreal ori);
    qreal getValue();

private:
    qreal scale;

    Ui::SettingScaleDialog *ui;
};

#endif // SETTINGSCALEDIALOG_H
