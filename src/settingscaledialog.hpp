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

    bool getScale(const QSize size, const qreal ori);
    qreal getValue() const;

private slots:
    void accepted_DialogButton();
    void rejected_DialogButton();
    void spinbox_valueChanged(const double d);

private:
    QGridLayout *layout;
    QDoubleSpinBox *spinbox;
    QLabel *desc_label;
    QLabel *height_label;
    QLabel *desc_height_label;
    QLabel *width_label;
    QLabel *desc_width_label;
    QDialogButtonBox *buttonbox;

    qreal scale;
    QSize img_size;
};

#endif // SETTINGSCALEDIALOG_H
