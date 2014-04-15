#include "benchmarkdialog.hpp"

BenchmarkDialog::BenchmarkDialog(QWidget *parent)
    : QDialog(parent,
              Qt::Sheet |
              Qt::WindowTitleHint |
              Qt::WindowCloseButtonHint)
    , layout(new QGridLayout())
    , nearest_text(new QLabel())
    , nearest_lcd(new QLCDNumber())
    , bilinear_text(new QLabel())
    , bilinear_lcd(new QLCDNumber())
    , bicubic_text(new QLabel())
    , bicubic_lcd(new QLCDNumber())
    , lanczos2_text(new QLabel())
    , lanczos2_lcd(new QLCDNumber())
    , lanczos3_text(new QLabel())
    , lanczos3_lcd(new QLCDNumber())
    , benchSize_text(new QLabel())
    , comboBox(new QComboBox())
    , startButton(new QPushButton())
    , buttonbox(new QDialogButtonBox(QDialogButtonBox::Close))
{
    setLayout(layout);
    setWindowTitle(tr("ベンチマーク"));

    nearest_text->setText(tr("NearestNeighbor"));
    nearest_lcd->setSegmentStyle(QLCDNumber::Flat);
    bilinear_text->setText(tr("Bilinear"));
    bilinear_lcd->setSegmentStyle(QLCDNumber::Flat);
    bicubic_text->setText(tr("Bicubic"));
    bicubic_lcd->setSegmentStyle(QLCDNumber::Flat);
    lanczos2_text->setText(tr("Lanczos2"));
    lanczos2_lcd->setSegmentStyle(QLCDNumber::Flat);
    lanczos3_text->setText(tr("Lanczos3"));
    lanczos3_lcd->setSegmentStyle(QLCDNumber::Flat);
    benchSize_text->setText(tr("サイズ"));
    comboBox->addItems(QStringList() << "1000" << "1500" << "2000" << "2500" << "3000");
    startButton->setText(tr("開始"));

    layout->addWidget(nearest_text, 0, 0);
    layout->addWidget(nearest_lcd, 0, 1, 1, 2);
    layout->addWidget(bilinear_text, 1, 0);
    layout->addWidget(bilinear_lcd, 1, 1, 1, 2);
    layout->addWidget(bicubic_text, 2, 0);
    layout->addWidget(bicubic_lcd, 2, 1, 1, 2);
    layout->addWidget(lanczos2_text, 3, 0);
    layout->addWidget(lanczos2_lcd, 3, 1, 1, 2);
    layout->addWidget(lanczos3_text, 4, 0);
    layout->addWidget(lanczos3_lcd, 4, 1, 1, 2);

    layout->addWidget(benchSize_text, 5, 0);
    layout->addWidget(comboBox, 5, 1);
    layout->addWidget(startButton, 5, 2);
    layout->addWidget(buttonbox, 6, 0, 1, 3);

    connect(startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
    connect(buttonbox, SIGNAL(rejected()), this, SLOT(rejectedButtonClicked()));
}

BenchmarkDialog::~BenchmarkDialog()
{
    delete layout;

    delete nearest_text;
    delete nearest_lcd;
    delete bilinear_text;
    delete bilinear_lcd;
    delete bicubic_text;
    delete bicubic_lcd;
    delete lanczos2_text;
    delete lanczos2_lcd;
    delete lanczos3_text;
    delete lanczos3_lcd;

    delete benchSize_text;
    delete comboBox;
    delete startButton;
    delete buttonbox;
}

void BenchmarkDialog::startButtonClicked()
{
    int img_size(comboBox->currentText().toInt());
    qint64 elapsed;
    QImage img(img_size, img_size, QImage::Format_ARGB32);
    ImageViewer viewer;
    QElapsedTimer timer;

    viewer.setInterpolationMode(ImageViewer::NearestNeighbor);
    timer.start();
    viewer.showImage(img);
    elapsed = timer.elapsed();
    nearest_lcd->display(static_cast<int>(elapsed));

    viewer.setInterpolationMode(ImageViewer::Bilinear);
    timer.start();
    viewer.showImage(img);
    elapsed = timer.elapsed();
    bilinear_lcd->display(static_cast<int>(elapsed));

    viewer.setInterpolationMode(ImageViewer::Bicubic);
    timer.start();
    viewer.showImage(img);
    elapsed = timer.elapsed();
    bicubic_lcd->display(static_cast<int>(elapsed));

    /*
    viewer.setInterpolationMode(ImageViewer::Lanczos2);
    timer.start();
    viewer.showImage(img);
    elapsed = timer.elapsed();
    lanczos2_lcd->display(static_cast<int>(elapsed));

    viewer.setInterpolationMode(ImageViewer::Lanczos3);
    timer.start();
    viewer.showImage(img);
    elapsed = timer.elapsed();
    lanczos3_lcd->display(static_cast<int>(elapsed));
    */
}

void BenchmarkDialog::rejectedButtonClicked()
{
    done(QDialog::Rejected);
}
