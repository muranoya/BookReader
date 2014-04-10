#include "settingscaledialog.h"

SettingScaleDialog::SettingScaleDialog(QWidget *parent)
    : QDialog(parent,
              Qt::Sheet |
              Qt::WindowTitleHint |
              Qt::WindowCloseButtonHint),
      layout(new QGridLayout()),
      spinbox(new QDoubleSpinBox()),
      label(new QLabel()),
      buttonbox(new QDialogButtonBox(
                    QDialogButtonBox::Ok|
                    QDialogButtonBox::Cancel,
                    Qt::Horizontal))
{
    setLayout(layout);
    setWindowTitle(tr("倍率の設定"));

    label->setText(tr("倍率"));

    spinbox->setMinimum(0.0001);
    spinbox->setMaximum(2.0);
    spinbox->setSingleStep(0.01);
    spinbox->setDecimals(4);

    buttonbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout->addWidget(label, 0, 0);
    layout->addWidget(spinbox, 0, 1);
    layout->addWidget(buttonbox, 1, 0, 1, 2, Qt::AlignRight);

    connect(buttonbox, SIGNAL(accepted()), this, SLOT(accepted_DialogButton()));
    connect(buttonbox, SIGNAL(rejected()), this, SLOT(rejected_DialogButton()));
}

SettingScaleDialog::~SettingScaleDialog()
{
    delete layout;
    delete spinbox;
    delete label;
    delete buttonbox;
}

bool SettingScaleDialog::getScale(qreal ori)
{
    spinbox->setValue(ori);
    exec();
    if (result() == QDialog::Accepted)
    {
        scale = static_cast<qreal>(spinbox->value());
        return true;
    }
    else
    {
        scale = ori;
        return false;
    }
}

qreal SettingScaleDialog::getValue()
{
    return scale;
}

void SettingScaleDialog::accepted_DialogButton()
{
    done(QDialog::Accepted);
}

void SettingScaleDialog::rejected_DialogButton()
{
    done(QDialog::Rejected);
}
