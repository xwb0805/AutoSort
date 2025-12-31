#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QMap>
#include "common.h"

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
class QComboBox;
class QListWidget;
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
    void addExcludedExtension();
    void removeExcludedExtension();
    void accept() override;

    // 多路径监控
    void addMonitorPath();
    void removeMonitorPath();
    void moveMonitorPathUp();
    void moveMonitorPathDown();

    // 正则表达式规则
    void addRegexRule();
    void removeRegexRule();
    void onRegexRuleCellChanged(int row, int column);

    // 冲突处理策略
    void onConflictActionChanged(int index);

private:
    void setupUI();
    void setupGeneralTab();
    void setupRulesTab();
    void setupExcludedTab();
    void setupAdvancedTab();  // 新增高级选项卡
    void loadSettings();
    void saveSettings();
    void updateRuleTable();
    void updateExcludedTable();
    void updateMonitorPathsList();  // 更新监控路径列表
    void updateRegexRulesTable();   // 更新正则表达式规则表
    bool validateSettings();

    QVBoxLayout *m_mainLayout;
    QTabWidget *m_tabWidget;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_applyButton;
    QPushButton *m_resetButton;

    // 通用设置
    QWidget *m_generalTab;
    QLineEdit *m_downloadPathEdit;
    QPushButton *m_browseButton;
    QCheckBox *m_autoStartCheckBox;
    QCheckBox *m_minimizeToTrayCheckBox;
    QCheckBox *m_startMinimizedCheckBox;
    QSpinBox *m_delaySpinBox;
    QCheckBox *m_enableNotificationsCheckBox;
    QComboBox *m_languageComboBox;  // 语言选择

    // 规则设置
    QWidget *m_rulesTab;
    QTableWidget *m_rulesTable;
    QPushButton *m_addRuleButton;
    QPushButton *m_removeRuleButton;
    QPushButton *m_resetRulesButton;
    QPushButton *m_importRulesButton;
    QPushButton *m_exportRulesButton;

    // 排除设置
    QWidget *m_excludedTab;
    QTableWidget *m_excludedTable;
    QPushButton *m_addExcludedButton;
    QPushButton *m_removeExcludedButton;

    // 高级设置
    QWidget *m_advancedTab;
    QListWidget *m_monitorPathsList;  // 监控路径列表
    QPushButton *m_addPathButton;
    QPushButton *m_removePathButton;
    QPushButton *m_moveUpButton;
    QPushButton *m_moveDownButton;
    QTableWidget *m_regexRulesTable;  // 正则表达式规则表
    QPushButton *m_addRegexButton;
    QPushButton *m_removeRegexButton;
    QComboBox *m_conflictActionCombo;  // 冲突处理策略

    QMap<QString, QString> m_customRules;
    QStringList m_excludedExtensions;
    QStringList m_monitorPaths;  // 监控路径
    QMap<QString, QString> m_regexRules;  // 正则表达式规则
    ConflictAction m_conflictAction;  // 冲突处理策略
    bool m_settingsChanged;
    FileOrganizer *m_fileOrganizer;
};

#endif // SETTINGSDIALOG_H