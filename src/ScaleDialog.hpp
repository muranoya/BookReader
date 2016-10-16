#ifndef SCALEDIALOG_HPP
#define SCALEDIALOG_HPP

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

class ScaleDialog : private QDialog
{
    Q_OBJECT
public:
    static bool getScale(const QSize &size,
            double org, double &rslt);

private slots:
    void spinbox_valueChanged(double d);

private:
    explicit ScaleDialog(QWidget *parent = 0);
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

#endif // SCALEDIALOG_HPP
