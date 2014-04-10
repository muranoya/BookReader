#include "versiondialog.h"

VersionDialog::VersionDialog(QWidget *parent) :
    QDialog(parent,
            Qt::Sheet |
            Qt::WindowTitleHint |
            Qt::WindowCloseButtonHint),
    layout(new QVBoxLayout()),
    label(new QLabel()),
    button(new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal))
{
    setLayout(layout);
    layout->addWidget(label);
    layout->addWidget(button);

    setWindowTitle(tr("バージョン情報"));

    label->setText(QString(tr("ソフトウェア名:%1\n"
                              "バージョン:%2"))
                   .arg(BookReader::SOFTWARE_NAME)
                   .arg(BookReader::SOFTWARE_VERSION));

    connect(button, SIGNAL(rejected()), this, SLOT(button_close()));
}

VersionDialog::~VersionDialog()
{
    delete layout;
    delete label;
    delete button;
}

void VersionDialog::button_close()
{
    close();
}
