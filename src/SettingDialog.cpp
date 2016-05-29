#include "SettingDialog.hpp"
#include "AppSettings.hpp"
#include "ImageViewer.hpp"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent,
            Qt::Sheet |
            Qt::WindowTitleHint |
            Qt::WindowCloseButtonHint)
    , layout(new QVBoxLayout())
    , buttonbox(new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel))
    , group_OpenDir(new QGroupBox(tr("ディレクトリを開く方法")))
    , open_rec_layout(new QGridLayout())
    , open_rec_dir_level_text(new QLabel(tr("サブディレクトリの深さ")))
    , open_rec_dir_level(new QSpinBox())
    , group_Slideshow(new QGroupBox(tr("スライドショー"), this))
    , slideshow_layout(new QGridLayout())
    , slideshow_interval_text(new QLabel(tr("スライドショーの間隔(ms)")))
    , slideshow_interval_value(new QSpinBox())
    , group_Prefetch(new QGroupBox(tr("画像ファイルのプリフェッチ"), this))
    , prefetch_layout(new QGridLayout())
    , prefetch_text(new QLabel(tr("画像ファイル数")))
    , prefetch_value(new QSpinBox())
{
    setWindowTitle(tr("設定"));
    setLayout(layout);

    group_OpenDir->setLayout(open_rec_layout);
    open_rec_dir_level->setRange(0, 999);
    open_rec_layout->addWidget(open_rec_dir_level_text, 0, 0, 1, 1);
    open_rec_layout->addWidget(open_rec_dir_level, 0, 1, 1, 1);

    group_Slideshow->setLayout(slideshow_layout);
    slideshow_interval_value->setRange(100, 60000);
    slideshow_interval_value->setSingleStep(1000);
    slideshow_layout->addWidget(slideshow_interval_text, 0, 0, 1, 1);
    slideshow_layout->addWidget(slideshow_interval_value, 0, 1, 1, 1);

    group_Prefetch->setLayout(prefetch_layout);
    prefetch_value->setRange(0, 1000);
    prefetch_value->setSingleStep(1);
    prefetch_layout->addWidget(prefetch_text, 0, 0, 1, 1);
    prefetch_layout->addWidget(prefetch_value, 0, 1, 1, 1);

    layout->addWidget(group_OpenDir);
    layout->addWidget(group_Slideshow);
    layout->addWidget(group_Prefetch);
    layout->addWidget(buttonbox);

    connect(buttonbox, SIGNAL(accepted()),
            this, SLOT(button_save()));
    connect(buttonbox, SIGNAL(rejected()),
            this, SLOT(button_cancel()));

    loadSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete open_rec_dir_level_text;
    delete open_rec_dir_level;
    delete open_rec_layout;
    delete group_OpenDir;

    delete slideshow_interval_text;
    delete slideshow_interval_value;
    delete slideshow_layout;
    delete group_Slideshow;

    delete prefetch_text;
    delete prefetch_value;
    delete prefetch_layout;
    delete group_Prefetch;

    delete buttonbox;
    delete layout;
}

void
SettingsDialog::button_save()
{
    saveSettings();
    close();
}

void
SettingsDialog::button_cancel()
{
    close();
}

void
SettingsDialog::loadSettings()
{
    open_rec_dir_level->setValue(AppSettings::viewer_open_dir_level);
    slideshow_interval_value->setValue(AppSettings::playlist_slideshow_interval);
    prefetch_value->setValue(AppSettings::playlist_prefetch);
}

void
SettingsDialog::saveSettings()
{
    AppSettings::viewer_open_dir_level = open_rec_dir_level->value();
    AppSettings::playlist_slideshow_interval = slideshow_interval_value->value();
    AppSettings::playlist_prefetch = prefetch_value->value();
}

