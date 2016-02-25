#include "versiondialog.hpp"

VersionDialog::VersionDialog(QWidget *parent) :
    QDialog(parent,
            Qt::Sheet |
            Qt::WindowTitleHint |
            Qt::WindowCloseButtonHint),
    layout(new QGridLayout()),
    nameDesc(new QLabel()), nameLabel(new QLabel()),
    verDesc(new QLabel()),  verLabel(new QLabel()),
    qtDesc(new QLabel()),   qtLabel(new QLabel()),
    button(new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal))
{
    setLayout(layout);

    nameDesc->setText(tr("ソフトウェア名:"));
    nameLabel->setText(BookReader::SOFTWARE_NAME);
    verDesc->setText(tr("バージョン:"));
    verLabel->setText(BookReader::SOFTWARE_VERSION);
    qtDesc->setText(tr("Qtバージョン:"));
    qtLabel->setText(QString(QT_VERSION_STR));

    layout->addWidget(nameDesc, 0, 0);
    layout->addWidget(nameLabel, 0, 1);
    layout->addWidget(verDesc, 1, 0);
    layout->addWidget(verLabel, 1, 1);
    layout->addWidget(qtDesc, 2, 0);
    layout->addWidget(qtLabel, 2, 1);
    layout->addWidget(button, 3, 0, 1, 2);

    setWindowTitle(tr("バージョン"));

    connect(button, SIGNAL(rejected()), this, SLOT(button_close()));
}

VersionDialog::~VersionDialog()
{
    delete layout;
    delete nameDesc; delete nameLabel;
    delete verDesc;  delete verLabel;
    delete qtDesc;   delete qtLabel;
    delete button;
}

void
VersionDialog::button_close()
{
    close();
}
