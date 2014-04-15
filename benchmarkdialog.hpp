#ifndef BENCHMARKDIALOG_H
#define BENCHMARKDIALOG_H

#include "imageviewer.hpp"

#include <QImage>
#include <QDialog>
#include <QLabel>
#include <QLCDNumber>
#include <QGridLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QElapsedTimer>

class BenchmarkDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BenchmarkDialog(QWidget *parent = 0);
    ~BenchmarkDialog();

private slots:
    void startButtonClicked();
    void rejectedButtonClicked();

private:
    QGridLayout *layout;

    QLabel *nearest_text;
    QLCDNumber *nearest_lcd;
    QLabel *bilinear_text;
    QLCDNumber *bilinear_lcd;
    QLabel *bicubic_text;
    QLCDNumber *bicubic_lcd;
    QLabel *lanczos2_text;
    QLCDNumber *lanczos2_lcd;
    QLabel *lanczos3_text;
    QLCDNumber *lanczos3_lcd;

    QLabel *benchSize_text;
    QComboBox *comboBox;
    QPushButton *startButton;
    QDialogButtonBox *buttonbox;
};

#endif // BENCHMARKDIALOG_H
