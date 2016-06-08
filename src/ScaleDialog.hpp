#ifndef SETTINGSCALEDIALOG_H
#define SETTINGSCALEDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

class ScaleDialog : private QDialog
{
    Q_OBJECT
public:
    static bool getScale(const QSize &size, const qreal org, qreal &rslt);

private slots:
    void spinbox_valueChanged(const double d);

private:
    ScaleDialog(QWidget *parent = 0);
    ~ScaleDialog();

    QGridLayout *layout;
    QDoubleSpinBox *spinbox;
    QLabel *desc_label;
    QLabel *height_label;
    QLabel *desc_height_label;
    QLabel *width_label;
    QLabel *desc_width_label;
    QDialogButtonBox *buttonbox;

    QSize img_size;
};

#endif // SETTINGSCALEDIALOG_H
