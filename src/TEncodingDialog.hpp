#ifndef TENCODINGDIALOG_H
#define TENCODINGDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVector>
#include <QTextCodec>

class TEncodingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TEncodingDialog(QWidget *parent = 0);
    virtual ~TEncodingDialog();
    
    QTextCodec *selectTextCodec(const QByteArray &src);

private slots:
    void searchTextChanged(const QString &text);
    void listItemSelectionChanged();
    void acceptedDialogButton();
    void rejectedDialogButton();

private:
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
