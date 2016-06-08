#include "ScaleDialog.hpp"

ScaleDialog::ScaleDialog(QWidget *parent)
    : QDialog(parent,
              Qt::Dialog |
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

    connect(buttonbox, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(buttonbox, SIGNAL(rejected()),
            this, SLOT(reject()));
    connect(spinbox,   SIGNAL(valueChanged(double)),
            this, SLOT(spinbox_valueChanged(double)));
}

ScaleDialog::~ScaleDialog()
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
ScaleDialog::getScale(const QSize &size, const qreal ori, qreal &rslt)
{
    ScaleDialog dlg;
    dlg.img_size = QSize(size);
    dlg.width_label->setText(QString::number(size.width()));
    dlg.height_label->setText(QString::number(size.height()));

    dlg.spinbox->setValue(ori);
    dlg.exec();
    if (dlg.result() == QDialog::Accepted)
    {
        rslt = static_cast<qreal>(dlg.spinbox->value());
        return true;
    }
    else
    {
        return false;
    }
}

void
ScaleDialog::spinbox_valueChanged(const double d)
{
    width_label->setText(QString::number(int(d * img_size.width())));
    height_label->setText(QString::number(int(d * img_size.height())));
}

