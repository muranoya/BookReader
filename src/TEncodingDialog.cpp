#include "TEncodingDialog.hpp"

TEncodingDialog::TEncodingDialog(QWidget *parent)
    : QDialog(parent,
            Qt::Sheet | 
            Qt::WindowTitleHint |
            Qt::WindowCloseButtonHint)
    , layout(new QGridLayout())
    , search_label(new QLabel(tr("検索:")))
    , search_box(new QLineEdit())
    , src_label(new QLabel(tr("変換前:")))
    , src_text(new QLabel())
    , dst_label(new QLabel(tr("変換後:")))
    , dst_text(new QLabel())
    , codec_list(new QListWidget())
    , buttonbox(new QDialogButtonBox(
                        QDialogButtonBox::Ok |
                        QDialogButtonBox::Cancel,
                        Qt::Horizontal))
{
    setLayout(layout);
    setWindowTitle(tr("コーデックの選択"));

    layout->addWidget(search_label, 0, 0, 1, 1);
    layout->addWidget(search_box, 0, 1, 1, 1);
    layout->addWidget(src_label, 1, 0, 1, 1);
    layout->addWidget(src_text, 1, 1, 1, 1);
    layout->addWidget(dst_label, 2, 0, 1, 1);
    layout->addWidget(dst_text, 2, 1, 1, 1);
    layout->addWidget(codec_list, 3, 0, 1, 2);
    layout->addWidget(buttonbox, 4, 0, 1, 2, Qt::AlignRight);

    QList<int> avail_codecs = QTextCodec::availableMibs();
    for (QList<int>::const_iterator i = avail_codecs.constBegin();
            i != avail_codecs.constEnd(); ++i)
    {
        QTextCodec *c = QTextCodec::codecForMib(*i);
        codec_list->addItem(new QListWidgetItem(QString(c->name())));
    }

    connect(search_box, SIGNAL(textChanged(const QString&)),
            this, SLOT(searchTextChanged(const QString&)));
    connect(codec_list, SIGNAL(itemSelectionChanged()),
            this, SLOT(listItemSelectionChanged()));
    connect(buttonbox, SIGNAL(accepted()),
            this, SLOT(acceptedDialogButton()));
    connect(buttonbox, SIGNAL(rejected()),
            this, SLOT(rejectedDialogButton()));
}

TEncodingDialog::~TEncodingDialog()
{
    codec_list->clear();
    delete buttonbox;
    delete codec_list;
    delete dst_text;
    delete dst_label;
    delete src_text;
    delete src_label;
    delete search_box;
    delete search_label;
    delete layout;
}

QTextCodec*
TEncodingDialog::selectTextCodec(const QByteArray &src)
{
    src_bytes = QByteArray(src);
    src_text->setText(QString(src_bytes));
    exec();
    if (result() == QDialog::Accepted)
    {
        return selectedCodec();
    }
    else
    {
        return nullptr;
    }
}

void
TEncodingDialog::searchTextChanged(const QString &text)
{
    int len = codec_list->count();
    for (int i = 0; i < len; ++i)
    {
        QListWidgetItem *item = codec_list->item(i);
        if (item->text().contains(text))
        {
            item->setHidden(false);
        }
        else
        {
            item->setHidden(true);
        }
    }
}

void
TEncodingDialog::listItemSelectionChanged()
{
    QTextCodec *c = selectedCodec();
    if (c != nullptr)
    {
        QString conv = c->toUnicode(src_bytes);
        dst_text->setText(conv);
    }
    else
    {
        dst_text->setText(QString());
    }
}

void
TEncodingDialog::acceptedDialogButton()
{
    done(QDialog::Accepted);
}

void
TEncodingDialog::rejectedDialogButton()
{
    done(QDialog::Rejected);
}

QTextCodec *
TEncodingDialog::selectedCodec() const
{
    QList<QListWidgetItem*> sitems = codec_list->selectedItems();
    if (sitems.isEmpty())
    {
        return nullptr;
    }
    else
    {
        return QTextCodec::codecForName(sitems.at(0)->text().toUtf8());
    }
}
