#include "settingsdialog.h"
#include "fileorganizer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGroupBox>
#include <QFileDialog>
#include <QSpinBox>
#include <QTextEdit>
#include <QHeaderView>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent, FileOrganizer *fileOrganizer)
    : QDialog(parent)
    , m_mainLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_applyButton(nullptr)
    , m_resetButton(nullptr)
    , m_settingsChanged(false)
    , m_fileOrganizer(fileOrganizer)
{
    setupUI();
    loadSettings();
    setWindowTitle("璁剧疆");
    setFixedSize(600, 500);
    setModal(true);
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // 鍒涘缓鏍囩椤垫帶浠?    m_tabWidget = new QTabWidget(this);
    
    setupGeneralTab();
    setupRulesTab();
    setupAdvancedTab();
    
    m_mainLayout->addWidget(m_tabWidget);
    
    // 鍒涘缓鎸夐挳甯冨眬
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_resetButton = new QPushButton("閲嶇疆榛樿", this);
    m_applyButton = new QPushButton("搴旂敤", this);
    m_okButton = new QPushButton("纭畾", this);
    m_cancelButton = new QPushButton("鍙栨秷", this);
    
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    m_mainLayout->addLayout(buttonLayout);
    
    // 杩炴帴淇″彿
    connect(m_okButton, &QPushButton::clicked, this, &SettingsDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::reject);
    connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);
    
    // 杩炴帴閫氱敤鏍囩椤电殑鎸夐挳淇″彿
    if (m_browseButton) {
        connect(m_browseButton, &QPushButton::clicked, this, &SettingsDialog::browseDownloadPath);
    }
    
    // 杩炴帴瑙勫垯鏍囩椤电殑鎸夐挳淇″彿锛堟鏌ユ寜閽槸鍚﹀凡鍒涘缓锛?    if (m_addRuleButton) {
        connect(m_addRuleButton, &QPushButton::clicked, this, &SettingsDialog::addRule);
    }
    if (m_removeRuleButton) {
        connect(m_removeRuleButton, &QPushButton::clicked, this, &SettingsDialog::removeRule);
    }
    if (m_resetRulesButton) {
        connect(m_resetRulesButton, &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);
    }
    if (m_importRulesButton) {
        connect(m_importRulesButton, &QPushButton::clicked, this, &SettingsDialog::importRules);
    }
    if (m_exportRulesButton) {
        connect(m_exportRulesButton, &QPushButton::clicked, this, &SettingsDialog::exportRules);
    }
    if (m_rulesTable) {
        connect(m_rulesTable, &QTableWidget::cellChanged, this, &SettingsDialog::onRuleCellChanged);
    }
}

void SettingsDialog::setupGeneralTab()
{
    m_generalTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_generalTab);
    
    // 涓嬭浇璺緞璁剧疆
    QGroupBox *pathGroup = new QGroupBox("涓嬭浇璺緞", this);
    QVBoxLayout *pathLayout = new QVBoxLayout(pathGroup);
    
    QHBoxLayout *pathEditLayout = new QHBoxLayout();
    m_downloadPathEdit = new QLineEdit(this);
    m_browseButton = new QPushButton("娴忚...", this);
    pathEditLayout->addWidget(m_downloadPathEdit);
    pathEditLayout->addWidget(m_browseButton);
    pathLayout->addLayout(pathEditLayout);
    
    layout->addWidget(pathGroup);
    
    // 鍚姩閫夐」
    QGroupBox *startupGroup = new QGroupBox("鍚姩閫夐」", this);
    QVBoxLayout *startupLayout = new QVBoxLayout(startupGroup);
    
    m_autoStartCheckBox = new QCheckBox("寮€鏈鸿嚜鍔ㄥ惎鍔?, this);
    m_minimizeToTrayCheckBox = new QCheckBox("鏈€灏忓寲鍒扮郴缁熸墭鐩?, this);
    m_startMinimizedCheckBox = new QCheckBox("鍚姩鏃舵渶灏忓寲", this);
    
    startupLayout->addWidget(m_autoStartCheckBox);
    startupLayout->addWidget(m_minimizeToTrayCheckBox);
    startupLayout->addWidget(m_startMinimizedCheckBox);
    
    layout->addWidget(startupGroup);
    
    // 鏁寸悊璁剧疆
    QGroupBox *organizeGroup = new QGroupBox("鏁寸悊璁剧疆", this);
    QVBoxLayout *organizeLayout = new QVBoxLayout(organizeGroup);
    
    QHBoxLayout *delayLayout = new QHBoxLayout();
    delayLayout->addWidget(new QLabel("鏁寸悊寤惰繜 (绉?:", this));
    m_delaySpinBox = new QSpinBox(this);
    m_delaySpinBox->setRange(1, 60);
    m_delaySpinBox->setValue(2);
    delayLayout->addWidget(m_delaySpinBox);
    delayLayout->addStretch();
    
    m_enableNotificationsCheckBox = new QCheckBox("鍚敤閫氱煡", this);
    
    organizeLayout->addLayout(delayLayout);
    organizeLayout->addWidget(m_enableNotificationsCheckBox);
    
    layout->addWidget(organizeGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_generalTab, "閫氱敤");
}

void SettingsDialog::setupRulesTab()
{
    m_rulesTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_rulesTab);
    
    // 瑙勫垯琛ㄦ牸
    QGroupBox *rulesGroup = new QGroupBox("鏂囦欢瑙勫垯", this);
    QVBoxLayout *rulesLayout = new QVBoxLayout(rulesGroup);
    
    m_rulesTable = new QTableWidget(0, 2, this);
    m_rulesTable->setHorizontalHeaderLabels({"鎵╁睍鍚?, "鐩爣鏂囦欢澶?});
    m_rulesTable->horizontalHeader()->setStretchLastSection(true);
    m_rulesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    rulesLayout->addWidget(m_rulesTable);
    
    // 瑙勫垯鎸夐挳
    QHBoxLayout *rulesButtonLayout = new QHBoxLayout();
    m_addRuleButton = new QPushButton("娣诲姞瑙勫垯", this);
    m_removeRuleButton = new QPushButton("鍒犻櫎瑙勫垯", this);
    m_resetRulesButton = new QPushButton("閲嶇疆榛樿", this);
    m_importRulesButton = new QPushButton("瀵煎叆瑙勫垯", this);
    m_exportRulesButton = new QPushButton("瀵煎嚭瑙勫垯", this);
    
    rulesButtonLayout->addWidget(m_addRuleButton);
    rulesButtonLayout->addWidget(m_removeRuleButton);
    rulesButtonLayout->addWidget(m_resetRulesButton);
    rulesButtonLayout->addWidget(m_importRulesButton);
    rulesButtonLayout->addWidget(m_exportRulesButton);
    
    rulesLayout->addLayout(rulesButtonLayout);
    layout->addWidget(rulesGroup);
    
    // 瑙勫垯棰勮
    QGroupBox *previewGroup = new QGroupBox("瑙勫垯棰勮", this);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    
    m_rulesPreview = new QTextEdit(this);
    m_rulesPreview->setReadOnly(true);
    m_rulesPreview->setMaximumHeight(150);
    
    previewLayout->addWidget(m_rulesPreview);
    layout->addWidget(previewGroup);
    
    m_tabWidget->addTab(m_rulesTab, "瑙勫垯");
}

void SettingsDialog::setupAdvancedTab()
{
    m_advancedTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_advancedTab);
    
    // 鏃ュ織璁剧疆
    QGroupBox *logGroup = new QGroupBox("鏃ュ織璁剧疆", this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    
    m_logToFileCheckBox = new QCheckBox("鍚敤鏃ュ織璁板綍", this);
    
    QHBoxLayout *logPathLayout = new QHBoxLayout();
    logPathLayout->addWidget(new QLabel("鏃ュ織璺緞:", this));
    m_logPathEdit = new QLineEdit(this);
    m_browseLogButton = new QPushButton("娴忚...", this);
    logPathLayout->addWidget(m_logPathEdit);
    logPathLayout->addWidget(m_browseLogButton);
    
    QHBoxLayout *logSizeLayout = new QHBoxLayout();
    logSizeLayout->addWidget(new QLabel("鏈€澶ф棩蹇楀ぇ灏?(MB):", this));
    m_maxLogSizeSpinBox = new QSpinBox(this);
    m_maxLogSizeSpinBox->setRange(1, 100);
    m_maxLogSizeSpinBox->setValue(10);
    logSizeLayout->addWidget(m_maxLogSizeSpinBox);
    logSizeLayout->addStretch();
    
    logLayout->addWidget(m_logToFileCheckBox);
    logLayout->addLayout(logPathLayout);
    logLayout->addLayout(logSizeLayout);
    
    layout->addWidget(logGroup);
    
    // 澶囦唤璁剧疆
    QGroupBox *backupGroup = new QGroupBox("澶囦唤璁剧疆", this);
    QVBoxLayout *backupLayout = new QVBoxLayout(backupGroup);
    
    m_backupFilesCheckBox = new QCheckBox("鏁寸悊鍓嶅浠芥枃浠?, this);
    
    QHBoxLayout *backupPathLayout = new QHBoxLayout();
    backupPathLayout->addWidget(new QLabel("澶囦唤璺緞:", this));
    m_backupPathEdit = new QLineEdit(this);
    m_browseBackupButton = new QPushButton("娴忚...", this);
    backupPathLayout->addWidget(m_backupPathEdit);
    backupPathLayout->addWidget(m_browseBackupButton);
    
    backupLayout->addWidget(m_backupFilesCheckBox);
    backupLayout->addLayout(backupPathLayout);
    
    layout->addWidget(backupGroup);
    layout->addStretch();
    
    m_tabWidget->addTab(m_advancedTab, "楂樼骇");
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    
    // 閫氱敤璁剧疆
    m_downloadPathEdit->setText(settings.value("downloadPath", 
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString());
    m_autoStartCheckBox->setChecked(settings.value("autoStart", false).toBool());
    m_minimizeToTrayCheckBox->setChecked(settings.value("minimizeToTray", true).toBool());
    m_startMinimizedCheckBox->setChecked(settings.value("startMinimized", false).toBool());
    m_delaySpinBox->setValue(settings.value("organizationDelay", 2).toInt());
    m_enableNotificationsCheckBox->setChecked(settings.value("enableNotifications", true).toBool());
    
    // 鍔犺浇鑷畾涔夎鍒?    settings.beginGroup("FileRules");
    m_customRules.clear();
    for (const QString &key : settings.childKeys()) {
        m_customRules[key] = settings.value(key).toString();
    }
    settings.endGroup();
    
    updateRuleTable();
    
    // 楂樼骇璁剧疆
    m_logToFileCheckBox->setChecked(settings.value("logToFile", false).toBool());
    m_logPathEdit->setText(settings.value("logPath", QDir::homePath() + "/autosort.log").toString());
    m_maxLogSizeSpinBox->setValue(settings.value("maxLogSize", 10).toInt());
    m_backupFilesCheckBox->setChecked(settings.value("backupFiles", false).toBool());
    m_backupPathEdit->setText(settings.value("backupPath", QDir::homePath() + "/autosort_backup").toString());
}

void SettingsDialog::saveSettings()
{
    if (!validateSettings()) {
        return;
    }
    
    QSettings settings;
    
    // 淇濆瓨閫氱敤璁剧疆
    settings.setValue("downloadPath", m_downloadPathEdit->text());
    settings.setValue("autoStart", m_autoStartCheckBox->isChecked());
    settings.setValue("minimizeToTray", m_minimizeToTrayCheckBox->isChecked());
    settings.setValue("startMinimized", m_startMinimizedCheckBox->isChecked());
    settings.setValue("organizationDelay", m_delaySpinBox->value());
    settings.setValue("enableNotifications", m_enableNotificationsCheckBox->isChecked());
    
    // 淇濆瓨鑷畾涔夎鍒?    settings.beginGroup("FileRules");
    settings.remove(""); // 娓呴櫎鐜版湁瑙勫垯
    for (auto it = m_customRules.constBegin(); it != m_customRules.constEnd(); ++it) {
        settings.setValue(it.key(), it.value());
    }
    settings.endGroup();
    
    // 淇濆瓨楂樼骇璁剧疆
    settings.setValue("logToFile", m_logToFileCheckBox->isChecked());
    settings.setValue("logPath", m_logPathEdit->text());
    settings.setValue("maxLogSize", m_maxLogSizeSpinBox->value());
    settings.setValue("backupFiles", m_backupFilesCheckBox->isChecked());
    settings.setValue("backupPath", m_backupPathEdit->text());
    
    // 閫氱煡 FileOrganizer 閲嶆柊鍔犺浇瑙勫垯
    if (m_fileOrganizer) {
        m_fileOrganizer->loadCustomRules();
    }
    
    m_settingsChanged = false;
    m_applyButton->setEnabled(false);
}

void SettingsDialog::browseDownloadPath()
{
    QString path = QFileDialog::getExistingDirectory(this, "閫夋嫨涓嬭浇鏂囦欢澶?, m_downloadPathEdit->text());
    if (!path.isEmpty()) {
        m_downloadPathEdit->setText(path);
        m_settingsChanged = true;
        m_applyButton->setEnabled(true);
    }
}

void SettingsDialog::addRule()
{
    if (!m_rulesTable) {
        qWarning() << "閿欒: rulesTable 涓?null";
        return;
    }
    
    // 纭繚琛ㄦ牸鏄彲缂栬緫鐨?    m_rulesTable->setEditTriggers(QAbstractItemView::DoubleClicked | 
                                   QAbstractItemView::EditKeyPressed |
                                   QAbstractItemView::AnyKeyPressed);
    
    int row = m_rulesTable->rowCount();
    qDebug() << "娣诲姞瑙勫垯鍒拌: " << row;
    
    // 鎻掑叆鏂拌
    m_rulesTable->insertRow(row);
    
    // 鍒涘缓鍗曞厓鏍奸」骞惰缃负鍙紪杈?    QTableWidgetItem *extItem = new QTableWidgetItem("");
    QTableWidgetItem *folderItem = new QTableWidgetItem("");
    
    extItem->setFlags(extItem->flags() | Qt::ItemIsEditable);
    folderItem->setFlags(folderItem->flags() | Qt::ItemIsEditable);
    
    m_rulesTable->setItem(row, 0, extItem);
    m_rulesTable->setItem(row, 1, folderItem);
    
    // 婊氬姩鍒版柊琛屽苟閫変腑
    m_rulesTable->scrollToItem(extItem);
    m_rulesTable->selectRow(row);
    
    // 寮€濮嬬紪杈戠涓€涓崟鍏冩牸
    m_rulesTable->editItem(extItem);
    
    m_settingsChanged = true;
    if (m_applyButton) {
        m_applyButton->setEnabled(true);
    }
    
    qDebug() << "瑙勫垯娣诲姞鎴愬姛锛屽綋鍓嶈鏁? " << m_rulesTable->rowCount();
}

void SettingsDialog::removeRule()
{
    int currentRow = m_rulesTable->currentRow();
    if (currentRow >= 0) {
        m_rulesTable->removeRow(currentRow);
        m_settingsChanged = true;
        m_applyButton->setEnabled(true);
    }
}

void SettingsDialog::resetToDefaults()
{
    if (QMessageBox::question(this, "閲嶇疆璁剧疆", "纭畾瑕侀噸缃墍鏈夎缃负榛樿鍊煎悧锛?,
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QSettings settings;
        settings.clear();
        loadSettings();
        m_settingsChanged = true;
        m_applyButton->setEnabled(true);
    }
}

void SettingsDialog::importRules()
{
    QString fileName = QFileDialog::getOpenFileName(this, "瀵煎叆瑙勫垯", "", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "瀵煎叆澶辫触", "鏃犳硶鎵撳紑鏂囦欢");
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("rules") && obj["rules"].isObject()) {
            QJsonObject rulesObj = obj["rules"].toObject();
            m_customRules.clear();
            for (auto it = rulesObj.constBegin(); it != rulesObj.constEnd(); ++it) {
                m_customRules[it.key()] = it.value().toString();
            }
            updateRuleTable();
            m_settingsChanged = true;
            m_applyButton->setEnabled(true);
        }
    }
}

void SettingsDialog::exportRules()
{
    QString fileName = QFileDialog::getSaveFileName(this, "瀵煎嚭瑙勫垯", "rules.json", "JSON Files (*.json)");
    if (fileName.isEmpty()) return;
    
    QJsonObject rootObj;
    QJsonObject rulesObj;
    
    for (auto it = m_customRules.constBegin(); it != m_customRules.constEnd(); ++it) {
        rulesObj[it.key()] = it.value();
    }
    
    rootObj["rules"] = rulesObj;
    rootObj["version"] = "1.0";
    rootObj["exported"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QJsonDocument doc(rootObj);
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    } else {
        QMessageBox::warning(this, "瀵煎嚭澶辫触", "鏃犳硶淇濆瓨鏂囦欢");
    }
}

void SettingsDialog::testOrganization()
{
    // TODO: 瀹炵幇娴嬭瘯鏁寸悊鍔熻兘
}

void SettingsDialog::onRuleCellChanged(int row, int column)
{
    QTableWidgetItem *extensionItem = m_rulesTable->item(row, 0);
    QTableWidgetItem *folderItem = m_rulesTable->item(row, 1);
    
    if (extensionItem && folderItem) {
        QString extension = extensionItem->text().trimmed().toLower();
        QString folder = folderItem->text().trimmed();
        
        if (!extension.isEmpty() && !folder.isEmpty()) {
            m_customRules[extension] = folder;
        } else {
            m_customRules.remove(extension);
        }
        
        updateRuleTable();
        m_settingsChanged = true;
        m_applyButton->setEnabled(true);
    }
}

void SettingsDialog::updateRuleTable()
{
    m_rulesTable->setRowCount(0);
    
    int row = 0;
    for (auto it = m_customRules.constBegin(); it != m_customRules.constEnd(); ++it) {
        m_rulesTable->insertRow(row);
        m_rulesTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        m_rulesTable->setItem(row, 1, new QTableWidgetItem(it.value()));
        row++;
    }
    
    // 鏇存柊棰勮
    QString previewText;
    for (auto it = m_customRules.constBegin(); it != m_customRules.constEnd(); ++it) {
        previewText += QString("*.%1 -> %2\n").arg(it.key(), it.value());
    }
    m_rulesPreview->setPlainText(previewText);
}

bool SettingsDialog::validateSettings()
{
    // 楠岃瘉涓嬭浇璺緞
    QString downloadPath = m_downloadPathEdit->text();
    if (!QDir(downloadPath).exists()) {
        QMessageBox::warning(this, "璁剧疆閿欒", "涓嬭浇璺緞涓嶅瓨鍦?);
        return false;
    }
    
    // 楠岃瘉鏃ュ織璺緞
    if (m_logToFileCheckBox->isChecked()) {
        QString logPath = m_logPathEdit->text();
        QFileInfo logFileInfo(logPath);
        QDir logDir = logFileInfo.absoluteDir();
        if (!logDir.exists()) {
            QMessageBox::warning(this, "璁剧疆閿欒", "鏃ュ織璺緞鐨勭洰褰曚笉瀛樺湪");
            return false;
        }
    }
    
    // 楠岃瘉澶囦唤璺緞
    if (m_backupFilesCheckBox->isChecked()) {
        QString backupPath = m_backupPathEdit->text();
        if (!QDir(backupPath).exists()) {
            QMessageBox::warning(this, "璁剧疆閿欒", "澶囦唤璺緞涓嶅瓨鍦?);
            return false;
        }
    }
    
    return true;
}