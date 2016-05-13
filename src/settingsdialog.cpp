#include "settingsdialog.hpp"
#include "appsettings.hpp"
#include "imageviewer.hpp"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent,
            Qt::Sheet |
            Qt::WindowTitleHint |
            Qt::WindowCloseButtonHint)
    , layout(new QVBoxLayout())
    , buttonbox(new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel))
    , group_InterpolatingPixel(new QGroupBox(tr("画素の補間方法"), this))
    , ipix_layout(new QVBoxLayout())
    , ipix_nearest(new QRadioButton(tr("Nearest Neighbor")))
    , ipix_bilinear(new QRadioButton(tr("Bilinear")))
    , ipix_bicubic(new QRadioButton(tr("Bicubic")))
    , group_OpenDir(new QGroupBox(tr("ディレクトリを開く方法")))
    , open_rec_layout(new QGridLayout())
    , open_rec_dir_level_text(new QLabel(tr("サブディレクトリの深さ")))
    , open_rec_dir_level(new QSpinBox())
    , group_Slideshow(new QGroupBox(tr("スライドショー"), this))
    , slideshow_layout(new QGridLayout())
    , slideshow_interval_text(new QLabel(tr("スライドショーの間隔(ms)")))
    , slideshow_interval_value(new QSpinBox())
{
    setWindowTitle(tr("設定"));
    setLayout(layout);

    group_InterpolatingPixel->setLayout(ipix_layout);
    ipix_layout->addWidget(ipix_nearest);
    ipix_layout->addWidget(ipix_bilinear);
    ipix_layout->addWidget(ipix_bicubic);

    group_OpenDir->setLayout(open_rec_layout);
    open_rec_dir_level->setRange(0, 999);
    open_rec_layout->addWidget(open_rec_dir_level_text, 0, 0, 1, 1);
    open_rec_layout->addWidget(open_rec_dir_level, 0, 1, 1, 1);

    group_Slideshow->setLayout(slideshow_layout);
    slideshow_interval_value->setRange(100, 60000);
    slideshow_interval_value->setSingleStep(1000);
    slideshow_layout->addWidget(slideshow_interval_text, 0, 0, 1, 1);
    slideshow_layout->addWidget(slideshow_interval_value, 0, 1, 1, 1);

    layout->addWidget(group_InterpolatingPixel);
    layout->addWidget(group_OpenDir);
    layout->addWidget(group_Slideshow);
    layout->addWidget(buttonbox);

    connect(buttonbox, SIGNAL(accepted()),
            this, SLOT(button_save()));
    connect(buttonbox, SIGNAL(rejected()),
            this, SLOT(button_cancel()));

    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ipix_nearest;
    delete ipix_bilinear;
    delete ipix_bicubic;
    delete ipix_layout;
    delete group_InterpolatingPixel;

    delete open_rec_dir_level_text;
    delete open_rec_dir_level;
    delete open_rec_layout;
    delete group_OpenDir;

    delete slideshow_interval_text;
    delete slideshow_interval_value;
    delete slideshow_layout;
    delete group_Slideshow;

    delete buttonbox;
    delete layout;
}

void
SettingsDialog::button_save()
{
    saveSettings();
    this->close();
}

void
SettingsDialog::button_cancel()
{
    this->close();
}

void
SettingsDialog::loadSettings()
{
    switch (AppSettings::viewer_ipixmode)
    {
    case ImageViewer::NearestNeighbor:
        ipix_nearest->setChecked(true);
        break;
    case ImageViewer::Bilinear:
        ipix_bilinear->setChecked(true);
        break;
    case ImageViewer::Bicubic:
        ipix_bicubic->setChecked(true);
        break;
    }

    open_rec_dir_level->setValue(AppSettings::main_open_dir_level);

    slideshow_interval_value->setValue(AppSettings::playlist_slideshow_interval);
}

void
SettingsDialog::saveSettings()
{
    if (ipix_nearest->isChecked())
    {
        AppSettings::viewer_ipixmode = int(ImageViewer::NearestNeighbor);
    }
    else if (ipix_bilinear->isChecked())
    {
        AppSettings::viewer_ipixmode = int(ImageViewer::Bilinear);
    }
    else if (ipix_bicubic->isChecked())
    {
        AppSettings::viewer_ipixmode = int(ImageViewer::Bicubic);
    }

    AppSettings::main_open_dir_level = open_rec_dir_level->value();

    AppSettings::playlist_slideshow_interval = slideshow_interval_value->value();
}
