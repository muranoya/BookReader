#ifndef TEXTENCDIALOG_H
#define TEXTENCDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVector>
#include <QTextCodec>

class TextEncDialog : private QDialog
{
    Q_OBJECT
public:
    static QTextCodec *selectTextCodec(const QByteArray &src);

private slots:
    void searchTextChanged(const QString &text);
    void listItemSelectionChanged();

private:
    TextEncDialog(QWidget *parent = 0);
    ~TextEncDialog();

    QGridLayout *layout;
    QLabel *search_label;
    QLineEdit *search_box;
    QLabel *src_label;
    QLabel *src_text;
    QLabel *dst_label;
    QLabel *dst_text;
    QListWidget *codec_list;
    QDialogButtonBox *buttonbox;
    QByteArray src_bytes;

    QTextCodec *selectedCodec() const;
};

#endif
