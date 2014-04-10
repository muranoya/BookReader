#ifndef SETTINGSCALEDIALOG_H
#define SETTINGSCALEDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

class SettingScaleDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingScaleDialog(QWidget *parent = 0);
    ~SettingScaleDialog();

    bool getScale(qreal ori);
    qreal getValue();

private slots:
    void accepted_DialogButton();
    void rejected_DialogButton();

private:
    QGridLayout *layout;
    QDoubleSpinBox *spinbox;
    QLabel *label;
    QDialogButtonBox *buttonbox;

    qreal scale;
};

#endif // SETTINGSCALEDIALOG_H
