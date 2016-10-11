#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>

class SettingDialog : private QDialog
{
    Q_OBJECT
public:
    static bool openSettingDialog();

private:
    SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

    QVBoxLayout *layout;
    QDialogButtonBox *buttonbox;

    QGroupBox   *group_OpenDir;
    QGridLayout *open_rec_layout;
    QLabel      *open_rec_dir_level_text;
    QSpinBox    *open_rec_dir_level;

    QGroupBox   *group_Prefetch;
    QGridLayout *prefetch_layout;
    QLabel      *prefetch_text;
    QSpinBox    *prefetch_value;

    QGroupBox    *group_FeedPage;
    QGridLayout  *feedpage_layout;
    QRadioButton *feedpage_clckbtn;
    QRadioButton *feedpage_clckpos;

    void loadSettings();
    void saveSettings();
};

#endif // SETTINGSDIALOG_H
