#include "mainwindow.h"
#include "fileorganizer.h"
#include "settingsdialog.h"
#include "statistics.h"
#include "logmanager.h"
#include "statisticsdialog.h"
#include "logviewerdialog.h"
#include "conflictresolutiondialog.h"
#include "filesearchdialog.h"
#include "batchrenamedialog.h"
#include "filefilterdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QMenu>
#include <QCloseEvent>
#include <QFileInfo>
#include <QTranslator>
#include <QLibraryInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_fileOrganizer(nullptr)
    , m_statistics(nullptr)
    , m_logManager(nullptr)
    , m_trayIcon(nullptr)
    , m_trayIconMenu(nullptr)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_statusLabel(nullptr)
    , m_organizeButton(nullptr)
    , m_settingsButton(nullptr)
    , m_statisticsButton(nullptr)
    , m_logViewerButton(nullptr)
    , m_fileSearchButton(nullptr)
    , m_batchRenameButton(nullptr)
    , m_fileFilterButton(nullptr)
    , m_toggleButton(nullptr)
    , m_isMonitoring(false)
{
    setupUI();
    createActions();
    createTrayIcon();

    m_fileOrganizer = new FileOrganizer(this);
    m_statistics = new Statistics(this);
    m_logManager = new LogManager(this);
    m_translator = new QTranslator(this);

    // 将统计模块传递给文件整理器
    m_fileOrganizer->setStatistics(m_statistics);

    connect(m_fileOrganizer, &FileOrganizer::organizationCompleted, this, &MainWindow::onOrganizationCompleted);
    connect(m_fileOrganizer, &FileOrganizer::fileConflict, this, &MainWindow::onFileConflict);

    loadSettings();

    m_logManager->addInfo("应用程序启动");

    setWindowTitle("自动整理工具");
    setFixedSize(500, 450);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        hide();
    }
}

MainWindow::~MainWindow()
{
    if (m_translator) {
        delete m_translator;
        m_translator = nullptr;
    }
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);

    m_statusLabel = new QLabel("状态: 未启动", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 20px; }");

    m_organizeButton = new QPushButton("立即整理", this);
    m_organizeButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_settingsButton = new QPushButton("设置", this);
    m_settingsButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_statisticsButton = new QPushButton("统计信息", this);
    m_statisticsButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_logViewerButton = new QPushButton("日志查看", this);
    m_logViewerButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_fileSearchButton = new QPushButton("文件搜索", this);
    m_fileSearchButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_batchRenameButton = new QPushButton("批量重命名", this);
    m_batchRenameButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_fileFilterButton = new QPushButton("文件过滤", this);
    m_fileFilterButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_toggleButton = new QPushButton("开始监控", this);
    m_toggleButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; background-color: #4CAF50; color: white; }");

    m_mainLayout->addWidget(m_statusLabel);
    m_mainLayout->addWidget(m_organizeButton);
    m_mainLayout->addWidget(m_settingsButton);
    m_mainLayout->addWidget(m_statisticsButton);
    m_mainLayout->addWidget(m_logViewerButton);
    m_mainLayout->addWidget(m_fileSearchButton);
    m_mainLayout->addWidget(m_batchRenameButton);
    m_mainLayout->addWidget(m_fileFilterButton);
    m_mainLayout->addWidget(m_toggleButton);
    m_mainLayout->addStretch();

    connect(m_organizeButton, &QPushButton::clicked, this, &MainWindow::organizeNow);
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
    connect(m_statisticsButton, &QPushButton::clicked, this, &MainWindow::showStatistics);
    connect(m_logViewerButton, &QPushButton::clicked, this, &MainWindow::showLogViewer);
    connect(m_fileSearchButton, &QPushButton::clicked, this, &MainWindow::showFileSearch);
    connect(m_batchRenameButton, &QPushButton::clicked, this, &MainWindow::showBatchRename);
    connect(m_fileFilterButton, &QPushButton::clicked, this, &MainWindow::showFileFilter);
    connect(m_toggleButton, &QPushButton::clicked, this, &MainWindow::toggleMonitoring);
}

void MainWindow::createActions()
{
    m_toggleAction = new QAction("开始监控", this);
    connect(m_toggleAction, &QAction::triggered, this, &MainWindow::toggleMonitoring);

    m_organizeAction = new QAction("立即整理", this);
    connect(m_organizeAction, &QAction::triggered, this, &MainWindow::organizeNow);

    m_settingsAction = new QAction("设置", this);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::showSettings);

    m_statisticsAction = new QAction("统计信息", this);
    connect(m_statisticsAction, &QAction::triggered, this, &MainWindow::showStatistics);

    m_logViewerAction = new QAction("日志查看", this);
    connect(m_logViewerAction, &QAction::triggered, this, &MainWindow::showLogViewer);

    m_fileSearchAction = new QAction("文件搜索", this);
    connect(m_fileSearchAction, &QAction::triggered, this, &MainWindow::showFileSearch);

    m_batchRenameAction = new QAction("批量重命名", this);
    connect(m_batchRenameAction, &QAction::triggered, this, &MainWindow::showBatchRename);

    m_fileFilterAction = new QAction("文件过滤", this);
    connect(m_fileFilterAction, &QAction::triggered, this, &MainWindow::showFileFilter);

    m_aboutAction = new QAction("关于", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    m_quitAction = new QAction("退出", this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWindow::createTrayIcon()
{
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addAction(m_toggleAction);
    m_trayIconMenu->addAction(m_organizeAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_settingsAction);
    m_trayIconMenu->addAction(m_statisticsAction);
    m_trayIconMenu->addAction(m_logViewerAction);
    m_trayIconMenu->addAction(m_fileSearchAction);
    m_trayIconMenu->addAction(m_batchRenameAction);
    m_trayIconMenu->addAction(m_fileFilterAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_aboutAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);
    m_trayIcon->setToolTip("自动整理工具");
    m_trayIcon->setIcon(QIcon(":/resources/icon.svg"));

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);

    m_trayIcon->show();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        setVisible(!isVisible());
    }
}

void MainWindow::showSettings()
{
    SettingsDialog dialog(this, m_fileOrganizer);
    if (dialog.exec() == QDialog::Accepted) {
        loadSettings();
    }
}

void MainWindow::organizeNow()
{
    if (!m_fileOrganizer) {
        QMessageBox::warning(this, "错误", "文件整理器未初始化");
        return;
    }

    QString downloadPath = m_fileOrganizer->downloadPath();
    qDebug() << "Starting file organization for path:" << downloadPath;

    if (downloadPath.isEmpty()) {
        QMessageBox::warning(this, "错误", "下载路径未设置");
        return;
    }

    QDir dir(downloadPath);
    if (!dir.exists()) {
        QMessageBox::warning(this, "错误", QString("下载路径不存在: %1").arg(downloadPath));
        return;
    }

    m_statusLabel->setText("状态: 正在整理...");
    QApplication::processEvents();

    int organizedCount = m_fileOrganizer->organizeFiles();

    qDebug() << "Organization completed. Files organized:" << organizedCount;

    m_statusLabel->setText(QString("状态: 已整理 %1 个文件").arg(organizedCount));

    if (m_trayIcon) {
        m_trayIcon->showMessage("整理完成", QString("已整理 %1 个文件").arg(organizedCount),
                               QSystemTrayIcon::Information, 3000);
    }

    if (organizedCount == 0) {
        m_logManager->addInfo("没有文件需要整理");
    } else {
        m_logManager->addSuccess(QString("成功整理 %1 个文件").arg(organizedCount));
    }
}

void MainWindow::toggleMonitoring()
{
    m_isMonitoring = !m_isMonitoring;

    if (m_isMonitoring) {
        m_fileOrganizer->startMonitoring();
        m_statusLabel->setText("状态: 监控中");
        m_toggleButton->setText("停止监控");
        m_toggleButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; background-color: #f44336; color: white; }");
        m_toggleAction->setText("停止监控");

        if (m_trayIcon) {
            m_trayIcon->showMessage("监控已开始", "正在监控下载文件夹", QSystemTrayIcon::Information, 2000);
        }
    } else {
        m_fileOrganizer->stopMonitoring();
        m_statusLabel->setText("状态: 已停止");
        m_toggleButton->setText("开始监控");
        m_toggleButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; background-color: #4CAF50; color: white; }");
        m_toggleAction->setText("开始监控");

        if (m_trayIcon) {
            m_trayIcon->showMessage("监控已停止", "文件监控已停止", QSystemTrayIcon::Information, 2000);
        }
    }

    saveSettings();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "关于",
                      "自动整理工具 v1.0\n\n"
                      "一个基于Qt的自动文件整理工具\n"
                      "按文件扩展名自动分类整理文件\n\n"
                      "特性:\n"
                      "• 后台运行\n"
                      "• 系统托盘集成\n"
                      "• 实时文件监控\n"
                      "• 自定义整理规则\n"
                      "• 文件冲突处理\n"
                      "• 正则表达式规则\n"
                      "• 多路径监控\n"
                      "• 统计信息\n"
                      "• 日志查看\n"
                      "• 文件预览\n"
                      "• 文件搜索\n"
                      "• 批量重命名\n"
                      "• 文件过滤");
}

void MainWindow::quitApplication()
{
    qApp->quit();
}

void MainWindow::onOrganizationCompleted(int count)
{
    m_statusLabel->setText(QString("状态: 已整理 %1 个文件").arg(count));
    m_logManager->addSuccess(QString("自动整理完成，共整理 %1 个文件").arg(count));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "自动整理工具",
                                     "是否隐藏到系统托盘？\n\n点击\"是\"隐藏到托盘，点击\"否\"退出程序。",
                                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (reply == QMessageBox::Yes) {
            hide();
            event->ignore();
            m_logManager->addInfo("程序已隐藏到系统托盘");
        } else if (reply == QMessageBox::No) {
            // 退出程序
            event->accept();
            m_logManager->addInfo("程序退出");
        } else {
            // 取消关闭
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;
    QString downloadPath = settings.value("downloadPath", QDir::homePath() + "/Downloads").toString();
    bool autoStart = settings.value("autoStart", false).toBool();

    // 加载语言设置
    QString language = settings.value("language", "zh_CN").toString();
    loadLanguage(language);

    if (m_fileOrganizer) {
        m_fileOrganizer->setDownloadPath(downloadPath);
        m_fileOrganizer->loadCustomRules();
    }

    if (autoStart && !m_isMonitoring) {
        toggleMonitoring();
    }
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("isMonitoring", m_isMonitoring);
}

void MainWindow::showStatistics()
{
    qDebug() << "MainWindow::showStatistics() called";
    qDebug() << "m_statistics pointer:" << m_statistics;

    if (!m_statistics) {
        QMessageBox::warning(this, "错误", "统计模块未初始化");
        return;
    }

    qDebug() << "Creating statistics dialog...";
    StatisticsDialog *dialog = new StatisticsDialog(m_statistics, this);

    qDebug() << "Showing statistics dialog...";
    dialog->show();
    dialog->raise();
    dialog->activateWindow();

    qDebug() << "Statistics dialog shown";
}

void MainWindow::showLogViewer()
{
    LogViewerDialog dialog(m_logManager, this);
    dialog.exec();
}

void MainWindow::showFileSearch()
{
    FileSearchDialog dialog(this);
    dialog.exec();
}

void MainWindow::showBatchRename()
{
    BatchRenameDialog dialog(this);
    dialog.exec();
}

void MainWindow::showFileFilter()
{
    FileFilterDialog dialog(this);
    dialog.exec();
}

void MainWindow::onFileConflict(const QString &sourcePath, const QString &targetPath)
{
    ConflictResolutionDialog dialog(QFileInfo(sourcePath), QFileInfo(targetPath), this);
    if (dialog.exec() == QDialog::Accepted) {
        ConflictAction action = dialog.getSelectedAction();
        QString newName = dialog.getNewName();

        // 处理用户选择
        if (action == ConflictAction::Overwrite) {
            QFile::remove(targetPath);
            QFile::rename(sourcePath, targetPath);
            m_logManager->addInfo(QString("覆盖文件: %1").arg(sourcePath));
        } else if (action == ConflictAction::OverwriteAll) {
            // 设置为全部覆盖模式
            m_fileOrganizer->setConflictAction(ConflictAction::OverwriteAll);
            QFile::remove(targetPath);
            QFile::rename(sourcePath, targetPath);
            m_logManager->addInfo(QString("覆盖文件（全部模式）: %1").arg(sourcePath));
        } else if (action == ConflictAction::Rename) {
            QString targetDir = QFileInfo(targetPath).absolutePath();
            QString newPath = targetDir + "/" + newName;
            QFile::rename(sourcePath, newPath);
            m_logManager->addInfo(QString("重命名文件: %1 -> %2").arg(sourcePath).arg(newPath));
        } else if (action == ConflictAction::Skip) {
            m_logManager->addInfo(QString("跳过文件: %1").arg(sourcePath));
        } else if (action == ConflictAction::SkipAll) {
            // 设置为全部跳过模式
            m_fileOrganizer->setConflictAction(ConflictAction::SkipAll);
            m_logManager->addInfo(QString("跳过文件（全部模式）: %1").arg(sourcePath));
        }
    }
}

void MainWindow::updateUI()
{
    // 更新状态标签
    if (m_isMonitoring) {
        m_statusLabel->setText(tr("状态: 正在监控..."));
    } else {
        m_statusLabel->setText(tr("状态: 未启动"));
    }

    // 更新按钮文本
    if (m_isMonitoring) {
        m_toggleButton->setText(tr("停止监控"));
        m_toggleAction->setText(tr("停止监控"));
    } else {
        m_toggleButton->setText(tr("开始监控"));
        m_toggleAction->setText(tr("开始监控"));
    }

    // 更新其他按钮文本
    m_organizeButton->setText(tr("立即整理"));
    m_settingsButton->setText(tr("设置"));
    m_statisticsButton->setText(tr("统计"));
    m_logViewerButton->setText(tr("日志"));
    m_fileSearchButton->setText(tr("搜索"));
    m_batchRenameButton->setText(tr("重命名"));
    m_fileFilterButton->setText(tr("过滤"));

    // 更新菜单项文本
    m_organizeAction->setText(tr("立即整理"));
    m_settingsAction->setText(tr("设置"));
    m_statisticsAction->setText(tr("统计"));
    m_logViewerAction->setText(tr("日志"));
    m_fileSearchAction->setText(tr("搜索"));
    m_batchRenameAction->setText(tr("重命名"));
    m_fileFilterAction->setText(tr("过滤"));
    m_aboutAction->setText(tr("关于"));
    m_quitAction->setText(tr("退出"));
}

void MainWindow::loadLanguage(const QString &language)
{
    if (m_translator) {
        qApp->removeTranslator(m_translator);
    }

    if (language == "en_US") {
        if (m_translator->load(":/translations/autosort_en")) {
            qApp->installTranslator(m_translator);
        }
    }
    // 默认使用中文，不需要加载翻译文件

    // 更新界面文本
    setWindowTitle(tr("自动整理工具"));
    updateUI();
}
