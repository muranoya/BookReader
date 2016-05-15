#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void button_save();
    void button_cancel();

private:
    QVBoxLayout *layout;
    QDialogButtonBox *buttonbox;

    QGroupBox *group_OpenDir;
    QGridLayout *open_rec_layout;
    QLabel *open_rec_dir_level_text;
    QSpinBox *open_rec_dir_level;

    QGroupBox *group_Slideshow;
    QGridLayout *slideshow_layout;
    QLabel *slideshow_interval_text;
    QSpinBox *slideshow_interval_value;

    void loadSettings();
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
