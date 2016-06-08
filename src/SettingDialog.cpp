#include "SettingDialog.hpp"
#include "AppSettings.hpp"
#include "ImageViewer.hpp"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent,
            Qt::Dialog |
            Qt::WindowTitleHint |
            Qt::WindowCloseButtonHint)
    , layout(new QVBoxLayout())
    , buttonbox(new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel))
    , group_OpenDir(new QGroupBox(tr("ディレクトリを開く方法")))
    , open_rec_layout(new QGridLayout())
    , open_rec_dir_level_text(new QLabel(tr("サブディレクトリの深さ")))
    , open_rec_dir_level(new QSpinBox())
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

    group_Prefetch->setLayout(prefetch_layout);
    prefetch_value->setRange(0, 1000);
    prefetch_value->setSingleStep(1);
    prefetch_layout->addWidget(prefetch_text, 0, 0, 1, 1);
    prefetch_layout->addWidget(prefetch_value, 0, 1, 1, 1);

    layout->addWidget(group_OpenDir);
    layout->addWidget(group_Prefetch);
    layout->addWidget(buttonbox);

    connect(buttonbox, SIGNAL(accepted()),
            this, SLOT(accept()));
    connect(buttonbox, SIGNAL(rejected()),
            this, SLOT(reject()));

    loadSettings();
}

SettingDialog::~SettingDialog()
{
    delete open_rec_dir_level_text;
    delete open_rec_dir_level;
    delete open_rec_layout;
    delete group_OpenDir;

    delete prefetch_text;
    delete prefetch_value;
    delete prefetch_layout;
    delete group_Prefetch;

    delete buttonbox;
    delete layout;
}

bool
SettingDialog::openSettingDialog()
{
    SettingDialog dlg;
    dlg.exec();
    return (dlg.result() == QDialog::Accepted);
}

void
SettingDialog::loadSettings()
{
    open_rec_dir_level->setValue(AppSettings::viewer_open_dir_level);
    prefetch_value->setValue(AppSettings::playlist_prefetch);
}

void
SettingDialog::saveSettings()
{
    AppSettings::viewer_open_dir_level = open_rec_dir_level->value();
    AppSettings::playlist_prefetch = prefetch_value->value();
}

