#include "settingscaledialog.hpp"

SettingScaleDialog::SettingScaleDialog(QWidget *parent)
    : QDialog(parent,
              Qt::Sheet |
              Qt::WindowTitleHint |
              Qt::WindowCloseButtonHint)
    , layout(new QGridLayout())
    , spinbox(new QDoubleSpinBox())
    , desc_label(new QLabel())
    , height_label(new QLabel())
    , desc_height_label(new QLabel())
    , width_label(new QLabel())
    , desc_width_label(new QLabel())
    , buttonbox(new QDialogButtonBox(
                    QDialogButtonBox::Ok |
                    QDialogButtonBox::Cancel,
                    Qt::Horizontal))
{
    setLayout(layout);
    setWindowTitle(tr("倍率の設定"));

    desc_label->setText(tr("倍率:"));
    desc_label->setAlignment(Qt::AlignCenter);

    spinbox->setMinimum(0.0001);
    spinbox->setMaximum(8.0);
    spinbox->setSingleStep(0.05);
    spinbox->setDecimals(3);

    width_label->setAlignment(Qt::AlignRight);
    desc_width_label->setText(tr("変更後の横幅:"));
    desc_width_label->setAlignment(Qt::AlignRight);
    height_label->setAlignment(Qt::AlignRight);
    desc_height_label->setText(tr("変更後の縦幅:"));
    desc_height_label->setAlignment(Qt::AlignRight);

    layout->addWidget(desc_label, 0, 0);
    layout->addWidget(spinbox, 0, 1);
    layout->addWidget(desc_width_label, 1, 0);
    layout->addWidget(width_label, 1, 1);
    layout->addWidget(desc_height_label, 2, 0);
    layout->addWidget(height_label, 2, 1);
    layout->addWidget(buttonbox, 3, 0, 1, 2, Qt::AlignRight);

    connect(buttonbox, SIGNAL(accepted()), this, SLOT(accepted_DialogButton()));
    connect(buttonbox, SIGNAL(rejected()), this, SLOT(rejected_DialogButton()));
    connect(spinbox, SIGNAL(valueChanged(double)), this, SLOT(spinbox_valueChanged(double)));
}

SettingScaleDialog::~SettingScaleDialog()
{
    delete layout;
    delete spinbox;
    delete desc_label;

    delete width_label;
    delete desc_width_label;

    delete height_label;
    delete desc_height_label;

    delete buttonbox;
}

bool
SettingScaleDialog::getScale(const QSize size, const qreal ori)
{
    img_size = size;
    width_label->setText(QString::number(size.width()));
    height_label->setText(QString::number(size.height()));

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

qreal
SettingScaleDialog::getValue() const
{
    return scale;
}

void
SettingScaleDialog::accepted_DialogButton()
{
    done(QDialog::Accepted);
}

void
SettingScaleDialog::rejected_DialogButton()
{
    done(QDialog::Rejected);
}

void
SettingScaleDialog::spinbox_valueChanged(const double d)
{
    width_label->setText(QString::number(int(d*img_size.width())));
    height_label->setText(QString::number(int(d*img_size.height())));
}
