#include "settingsdialog.h"
#include "fileorganizer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QFileDialog>
#include <QSpinBox>
#include <QMessageBox>
#include <QSettings>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>

SettingsDialog::SettingsDialog(QWidget *parent, FileOrganizer *fileOrganizer)
    : QDialog(parent)
    , m_fileOrganizer(fileOrganizer)
    , m_mainLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_applyButton(nullptr)
    , m_resetButton(nullptr)
    , m_generalTab(nullptr)
    , m_downloadPathEdit(nullptr)
    , m_browseButton(nullptr)
    , m_autoStartCheckBox(nullptr)
    , m_minimizeToTrayCheckBox(nullptr)
    , m_startMinimizedCheckBox(nullptr)
    , m_delaySpinBox(nullptr)
    , m_enableNotificationsCheckBox(nullptr)
    , m_rulesTab(nullptr)
    , m_rulesTable(nullptr)
    , m_addRuleButton(nullptr)
    , m_removeRuleButton(nullptr)
    , m_resetRulesButton(nullptr)
    , m_importRulesButton(nullptr)
    , m_exportRulesButton(nullptr)
    , m_settingsChanged(false)
{
    setupUI();
    loadSettings();
    setWindowTitle("设置");
    setMinimumSize(600, 450);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    m_tabWidget = new QTabWidget(this);
    setupGeneralTab();
    setupRulesTab();

    m_mainLayout->addWidget(m_tabWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_okButton = new QPushButton("确定", this);
    m_cancelButton = new QPushButton("取消", this);
    m_applyButton = new QPushButton("应用", this);
    m_resetButton = new QPushButton("重置默认", this);

    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_resetButton);

    m_mainLayout->addLayout(buttonLayout);

    connect(m_okButton, &QPushButton::clicked, this, &SettingsDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);
}

void SettingsDialog::setupGeneralTab()
{
    m_generalTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_generalTab);

    QGroupBox *pathGroup = new QGroupBox("下载路径", m_generalTab);
    QGridLayout *pathLayout = new QGridLayout(pathGroup);

    m_downloadPathEdit = new QLineEdit(m_generalTab);
    m_downloadPathEdit->setReadOnly(true);
    m_browseButton = new QPushButton("浏览...", m_generalTab);

    pathLayout->addWidget(new QLabel("路径:", m_generalTab), 0, 0);
    pathLayout->addWidget(m_downloadPathEdit, 0, 1);
    pathLayout->addWidget(m_browseButton, 0, 2);

    layout->addWidget(pathGroup);

    QGroupBox *behaviorGroup = new QGroupBox("行为设置", m_generalTab);
    QVBoxLayout *behaviorLayout = new QVBoxLayout(behaviorGroup);

    m_autoStartCheckBox = new QCheckBox("开机自动启动", m_generalTab);
    m_minimizeToTrayCheckBox = new QCheckBox("最小化到系统托盘", m_generalTab);
    m_startMinimizedCheckBox = new QCheckBox("启动时最小化", m_generalTab);
    m_enableNotificationsCheckBox = new QCheckBox("启用通知", m_generalTab);

    behaviorLayout->addWidget(m_autoStartCheckBox);
    behaviorLayout->addWidget(m_minimizeToTrayCheckBox);
    behaviorLayout->addWidget(m_startMinimizedCheckBox);
    behaviorLayout->addWidget(m_enableNotificationsCheckBox);

    layout->addWidget(behaviorGroup);

    QGroupBox *delayGroup = new QGroupBox("整理延迟", m_generalTab);
    QHBoxLayout *delayLayout = new QHBoxLayout(delayGroup);

    delayLayout->addWidget(new QLabel("文件变动后延迟整理（秒）:", m_generalTab));
    m_delaySpinBox = new QSpinBox(m_generalTab);
    m_delaySpinBox->setMinimum(1);
    m_delaySpinBox->setMaximum(300);
    m_delaySpinBox->setValue(5);
    delayLayout->addWidget(m_delaySpinBox);
    delayLayout->addStretch();

    layout->addWidget(delayGroup);

    layout->addStretch();

    m_tabWidget->addTab(m_generalTab, "通用");

    connect(m_browseButton, &QPushButton::clicked, this, &SettingsDialog::browseDownloadPath);
}

void SettingsDialog::setupRulesTab()
{
    m_rulesTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_rulesTab);

    QGroupBox *rulesGroup = new QGroupBox("文件分类规则", m_rulesTab);
    QVBoxLayout *rulesLayout = new QVBoxLayout(rulesGroup);

    m_rulesTable = new QTableWidget(m_rulesTab);
    m_rulesTable->setColumnCount(2);
    m_rulesTable->setHorizontalHeaderLabels(QStringList() << "扩展名" << "目标文件夹");
    m_rulesTable->horizontalHeader()->setStretchLastSection(true);
    m_rulesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_rulesTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    rulesLayout->addWidget(m_rulesTable);

    QHBoxLayout *ruleButtonsLayout = new QHBoxLayout();

    m_addRuleButton = new QPushButton("添加规则", m_rulesTab);
    m_removeRuleButton = new QPushButton("删除规则", m_rulesTab);
    m_resetRulesButton = new QPushButton("重置默认", m_rulesTab);

    ruleButtonsLayout->addWidget(m_addRuleButton);
    ruleButtonsLayout->addWidget(m_removeRuleButton);
    ruleButtonsLayout->addWidget(m_resetRulesButton);
    ruleButtonsLayout->addStretch();

    QHBoxLayout *importExportLayout = new QHBoxLayout();

    m_importRulesButton = new QPushButton("导入规则", m_rulesTab);
    m_exportRulesButton = new QPushButton("导出规则", m_rulesTab);

    importExportLayout->addWidget(m_importRulesButton);
    importExportLayout->addWidget(m_exportRulesButton);
    importExportLayout->addStretch();

    layout->addWidget(rulesGroup);
    layout->addLayout(ruleButtonsLayout);
    layout->addLayout(importExportLayout);

    m_tabWidget->addTab(m_rulesTab, "规则");

    connect(m_addRuleButton, &QPushButton::clicked, this, &SettingsDialog::addRule);
    connect(m_removeRuleButton, &QPushButton::clicked, this, &SettingsDialog::removeRule);
    connect(m_resetRulesButton, &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);
    connect(m_importRulesButton, &QPushButton::clicked, this, &SettingsDialog::importRules);
    connect(m_exportRulesButton, &QPushButton::clicked, this, &SettingsDialog::exportRules);
    connect(m_rulesTable, &QTableWidget::cellChanged, this, &SettingsDialog::onRuleCellChanged);
}

void SettingsDialog::browseDownloadPath()
{
    QString path = QFileDialog::getExistingDirectory(this, "选择下载文件夹", m_downloadPathEdit->text());
    if (!path.isEmpty()) {
        m_downloadPathEdit->setText(path);
        m_settingsChanged = true;
    }
}

void SettingsDialog::addRule()
{
    m_rulesTable->blockSignals(true);

    int row = m_rulesTable->rowCount();
    m_rulesTable->insertRow(row);

    QTableWidgetItem *extItem = new QTableWidgetItem("");
    QTableWidgetItem *folderItem = new QTableWidgetItem("");

    m_rulesTable->setItem(row, 0, extItem);
    m_rulesTable->setItem(row, 1, folderItem);

    m_rulesTable->blockSignals(false);

    m_rulesTable->selectRow(row);
    m_rulesTable->editItem(extItem);
    m_settingsChanged = true;
}

void SettingsDialog::removeRule()
{
    int currentRow = m_rulesTable->currentRow();
    if (currentRow >= 0) {
        m_rulesTable->removeRow(currentRow);
        m_settingsChanged = true;
    }
}

void SettingsDialog::resetToDefaults()
{
    if (QMessageBox::question(this, "确认", "确定要重置为默认规则吗？",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_fileOrganizer) {
            m_fileOrganizer->initializeDefaultRules();
            m_fileOrganizer->saveCustomRules();
            updateRuleTable();
            m_settingsChanged = true;
        }
    }
}

void SettingsDialog::importRules()
{
    QString fileName = QFileDialog::getOpenFileName(this, "导入规则", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "错误", "无效的规则文件格式");
        return;
    }

    if (m_fileOrganizer) {
        QString rulesPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(rulesPath);
        rulesPath += "/rules.json";

        QFile rulesFile(rulesPath);
        if (!rulesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "无法保存规则");
            return;
        }

        rulesFile.write(doc.toJson());
        rulesFile.close();

        m_fileOrganizer->loadCustomRules();
        updateRuleTable();
        QMessageBox::information(this, "成功", "规则导入成功");
        m_settingsChanged = true;
    }
}

void SettingsDialog::exportRules()
{
    if (m_fileOrganizer) {
        m_fileOrganizer->saveCustomRules();

        QString fileName = QFileDialog::getSaveFileName(this, "导出规则", "", "JSON Files (*.json)");
        if (fileName.isEmpty()) {
            return;
        }

        QString rulesPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        rulesPath += "/rules.json";

        QFile sourceFile(rulesPath);
        if (!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "无法读取规则");
            return;
        }

        QByteArray data = sourceFile.readAll();
        sourceFile.close();

        QFile destFile(fileName);
        if (!destFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "无法保存文件");
            return;
        }

        destFile.write(data);
        destFile.close();

        QMessageBox::information(this, "成功", "规则导出成功");
    }
}

void SettingsDialog::onRuleCellChanged(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    m_settingsChanged = true;
}

void SettingsDialog::accept()
{
    if (validateSettings()) {
        saveSettings();
        QDialog::accept();
    }
}

void SettingsDialog::loadSettings()
{
    QSettings settings;

    QString downloadPath = settings.value("downloadPath", QDir::homePath() + "/Downloads").toString();
    m_downloadPathEdit->setText(downloadPath);

    m_autoStartCheckBox->setChecked(settings.value("autoStart", false).toBool());
    m_minimizeToTrayCheckBox->setChecked(settings.value("minimizeToTray", true).toBool());
    m_startMinimizedCheckBox->setChecked(settings.value("startMinimized", false).toBool());
    m_enableNotificationsCheckBox->setChecked(settings.value("enableNotifications", true).toBool());
    m_delaySpinBox->setValue(settings.value("organizeDelay", 5).toInt());

    updateRuleTable();
}

void SettingsDialog::saveSettings()
{
    QSettings settings;

    settings.setValue("downloadPath", m_downloadPathEdit->text());
    settings.setValue("autoStart", m_autoStartCheckBox->isChecked());
    settings.setValue("minimizeToTray", m_minimizeToTrayCheckBox->isChecked());
    settings.setValue("startMinimized", m_startMinimizedCheckBox->isChecked());
    settings.setValue("enableNotifications", m_enableNotificationsCheckBox->isChecked());
    settings.setValue("organizeDelay", m_delaySpinBox->value());

    if (m_fileOrganizer) {
        m_fileOrganizer->setDownloadPath(m_downloadPathEdit->text());

        QMap<QString, QString> rules;
        for (int row = 0; row < m_rulesTable->rowCount(); ++row) {
            QTableWidgetItem *extItem = m_rulesTable->item(row, 0);
            QTableWidgetItem *folderItem = m_rulesTable->item(row, 1);

            if (extItem && folderItem && !extItem->text().isEmpty() && !folderItem->text().isEmpty()) {
                QString ext = extItem->text().toLower().trimmed();
                if (ext.startsWith(".")) {
                    ext = ext.mid(1);
                }
                rules[ext] = folderItem->text().trimmed();
            }
        }

        m_fileOrganizer->initializeDefaultRules();
        for (auto it = rules.begin(); it != rules.end(); ++it) {
            m_fileOrganizer->addCustomRule(it.key(), it.value());
        }
        m_fileOrganizer->saveCustomRules();
    }

    m_settingsChanged = false;
}

void SettingsDialog::updateRuleTable()
{
    m_rulesTable->setRowCount(0);

    if (m_fileOrganizer) {
        QMap<QString, QString> rules = m_fileOrganizer->customRules();
        m_rulesTable->blockSignals(true);

        for (auto it = rules.begin(); it != rules.end(); ++it) {
            int row = m_rulesTable->rowCount();
            m_rulesTable->insertRow(row);

            m_rulesTable->setItem(row, 0, new QTableWidgetItem(it.key()));
            m_rulesTable->setItem(row, 1, new QTableWidgetItem(it.value()));
        }

        m_rulesTable->blockSignals(false);
    }
}

bool SettingsDialog::validateSettings()
{
    QString downloadPath = m_downloadPathEdit->text();
    if (downloadPath.isEmpty()) {
        QMessageBox::warning(this, "错误", "下载路径不能为空");
        return false;
    }

    QDir dir(downloadPath);
    if (!dir.exists()) {
        QMessageBox::warning(this, "错误", "下载路径不存在");
        return false;
    }

    return true;
}