#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMap>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QTabWidget;
class QWidget;
class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QTableWidget;
class QTableWidgetItem;
class QGroupBox;
class QFileDialog;
class QSpinBox;
class QTextEdit;
QT_END_NAMESPACE

class FileOrganizer;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr, FileOrganizer *fileOrganizer = nullptr);
    ~SettingsDialog();

private slots:
    void browseDownloadPath();
    void addRule();
    void removeRule();
    void resetToDefaults();
    void importRules();
    void exportRules();
    void onRuleCellChanged(int row, int column);
    void accept() override;

private:
    void setupUI();
    void setupGeneralTab();
    void setupRulesTab();
    void loadSettings();
    void saveSettings();
    void updateRuleTable();
    bool validateSettings();

    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_applyButton;
    QPushButton *m_resetButton;

    QWidget *m_generalTab;
    QLineEdit *m_downloadPathEdit;
    QPushButton *m_browseButton;
    QCheckBox *m_autoStartCheckBox;
    QCheckBox *m_minimizeToTrayCheckBox;
    QCheckBox *m_startMinimizedCheckBox;
    QSpinBox *m_delaySpinBox;
    QCheckBox *m_enableNotificationsCheckBox;

    QWidget *m_rulesTab;
    QTableWidget *m_rulesTable;
    QPushButton *m_addRuleButton;
    QPushButton *m_removeRuleButton;
    QPushButton *m_resetRulesButton;
    QPushButton *m_importRulesButton;
    QPushButton *m_exportRulesButton;

    QMap<QString, QString> m_customRules;
    bool m_settingsChanged;
    FileOrganizer *m_fileOrganizer;
};

#endif // SETTINGSDIALOG_H